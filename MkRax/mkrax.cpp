//
//  main.cpp
//  MkRax
//
//  Created by 河田 智明 on 8/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//


#include "stdafx.h"
#include <signal.h>

static const uint8_t g_magicNumber[]={0x08, 0xde, 0x0f, 0x42};
static const uint64_t g_rawFlag=0x8000000000000000ULL;

static std::string g_outputFilename;
static int g_blockSizeShift=16;
static size_t g_blockSize;

#pragma mark - Processor

static FILE *g_outputFile=NULL;
static bool g_interrupted=false;
static uint64_t g_indexOffset=24;
static uint64_t g_inputTotalSize=0;
static std::vector<uint64_t> g_indices;

// here's some statistics.
static uint64_t g_rawBlocks=0;
static uint64_t g_lzmaBlocks=0;
static uint64_t g_totalBlocks=0;
static uint64_t g_outputTotalSize=0;

static void writeBE32(uint32_t value){
	uint8_t buf[4];
	buf[0]=(uint8_t)(value>>24);
	buf[1]=(uint8_t)(value>>16);
	buf[2]=(uint8_t)(value>>8);
	buf[3]=(uint8_t)(value);
	fwrite(buf, 1, 4, g_outputFile);
}

static void writeBE64(uint64_t value){
	writeBE32((uint32_t)(value>>32));
	writeBE32((uint32_t)(value));
}

static void writeDummyHeader(){
	// 24-byte.
	char buf[16];
	fwrite(buf, 1, 24, g_outputFile);
}

static void writeActualHeader(){
	fseek(g_outputFile, 0, SEEK_SET);
	fwrite(g_magicNumber, 1, 4, g_outputFile);
	writeBE32(g_blockSizeShift);
	writeBE64(g_inputTotalSize);
	writeBE64(g_indexOffset);
}

static void writeIndices(){
	for(size_t i=0;i<g_indices.size();i++){
		writeBE64(g_indices[i]);
	}
}

static void printStatistics(){
	puts("**** RAX STATISTICS ****");
	printf("Input Bytes: %llu B\n", g_inputTotalSize);
	printf("Output Bytes: %llu B\n", g_outputTotalSize);
	printf("Ratio: %.2f%c\n", (double)g_outputTotalSize/(double)g_inputTotalSize*100.,
		   '%');
	printf("Block Size: %u B\n", (unsigned int)g_blockSize);
	printf("XZ Blocks: %llu (%llu B Input)\n", g_lzmaBlocks,
		   g_lzmaBlocks*(uint64_t)g_blockSize);
	printf("Raw Blocks: %llu (%llu B Input)\n", g_rawBlocks,
		   g_rawBlocks*(uint64_t)g_blockSize);
	printf("Total Blocks: %llu\n", g_totalBlocks);
}

static void handleInterrupt(int){
	g_interrupted=true;
}

static void doProcess(){
	g_outputFile=fopen(g_outputFilename.c_str(), "wb");
	if(!g_outputFile){
		fprintf(stderr, "rax: error: couldn't open \"%s\" for writing\n", g_outputFilename.c_str());
		exit(2);
	}
	
	writeDummyHeader();
	
	// calculate actual block size, and allocate.
	g_blockSize=1<<g_blockSizeShift;
	
	char *inputBuffer;
	char *outputBuffer;
	size_t inputSize;
	size_t outputSize;
	inputBuffer=new char[g_blockSize];
	outputBuffer=new char[g_blockSize*2+16384];
	
	// info.
	printf("rax: using %d bytes block.\n", (int)g_blockSize);
	
	// setup interruption handler.
	signal(SIGINT, handleInterrupt);
	
	while((inputSize=fread(inputBuffer, 1, g_blockSize, stdin))>0 && !g_interrupted){
		
		assert((g_inputTotalSize&(g_blockSize-1))==0);
		
		g_inputTotalSize+=inputSize;
		
		uint64_t index=g_indexOffset;
		
		// try compression.
		outputSize=0;
		
		if(lzma_easy_buffer_encode(1, LZMA_CHECK_CRC32, NULL,
								   (const uint8_t *)inputBuffer, inputSize,
								   (uint8_t *)outputBuffer, &outputSize,
								   g_blockSize*2+16384)!=LZMA_OK){
			fprintf(stderr, "rax: error in liblzma.\n");
			exit(3);
		}
		
		if(outputSize>=inputSize){
			// not compressed...
			// write raw data.
			index|=g_rawFlag;
			fwrite(inputBuffer, 1, inputSize, g_outputFile);
			g_indexOffset+=inputSize;
			g_rawBlocks++;
		}else{
			// compressed!
			// write compressed data.
			fwrite(outputBuffer, 1, outputSize, g_outputFile);
			g_indexOffset+=outputSize;
			g_lzmaBlocks++;
		}
		
		g_totalBlocks++;
		
		g_indices.push_back(index);
		
	}
	
	g_outputTotalSize=g_indexOffset+8*(uint64_t)g_indices.size();
	
	puts("rax: writing indices...");
	writeIndices();
	
	
	puts("rax: writing Rax header...");
	writeActualHeader();
	
	delete[] inputBuffer;
	delete[] outputBuffer;
	
	printStatistics();
	
}

#pragma mark - Command-line

static void printHelp();

static void helpCommandLineHandler(const char *){
	printHelp();
	exit(0);
}


static void outputFilenameCommandLineHandler(const char *parameter){
	g_outputFilename=parameter;
}

static void blockSizeCommandLineHandler(const char *parameter){
	int bs=atoi(parameter);
	
	if(bs<1){
		fprintf(stderr, "rax: error: too small block size.\n");
		printHelp();
		exit(1);
	}
	if(bs>22){
		fprintf(stderr, "rax: error: too big block size (>22).\n");
		printHelp();
		exit(1);
	}
	
	g_blockSizeShift=bs;
}

static void handleInputFile(const char *fn){
	fprintf(stderr, "rax: error: specify input file as standard input.\n");
	printHelp();
	exit(1);
}

typedef void (*XTBCommandLineHandler)(const char *);

struct XTBCommandLineOption{
	char shortName;
	bool requiresParameter;
	XTBCommandLineHandler handler;
};

static const XTBCommandLineOption g_commandLineOptions[]={
	{'o', true, outputFilenameCommandLineHandler},
	{'h', false, helpCommandLineHandler},
	{'b', true, blockSizeCommandLineHandler},
	{0, false, NULL}
};

static void printHelp(){
	puts("MkRax [OPTIONS...] < INFILE");
	puts("Compress a file for random access in XTBook");
	puts("");
	puts("USAGE:");
	puts("");
	puts("-o OUTFILE.rax");
	puts("    Specifies the output path.");
	puts("");
	puts("-b SHIFT");
	puts("    Specifies the block size.");
	puts("... 16 = 64KB (default)");
	puts("    17 = 128KB");
	puts("    18 = 256KB");
	puts("    19 = 512KB ...");
	
	
	
}

static const XTBCommandLineOption *commandLineOptionForShortName(char c){
	for(int i=0;g_commandLineOptions[i].shortName;i++)
		if(g_commandLineOptions[i].shortName==c)
			return &(g_commandLineOptions[i]);
	return NULL;
}

static void parseCommandLine(int argc, const char **argv){
	for(int i=1;i<argc;i++){
		const char *arg=argv[i];
		if(arg[0]=='-'){
			for(int j=1;arg[j];j++){
				char c=arg[j];
				const XTBCommandLineOption *opt=commandLineOptionForShortName(c);
				if(opt==NULL){
					fprintf(stderr, "error: unknown option: %c\n", c);
					printHelp();
					exit(1);
				}
				if(opt->requiresParameter){
					const char *param=NULL;
					if(arg[j+1]==0){
						// get param from the next argv
						if(i<argc-1)
							param=argv[i+1];
						i++; // skip
					}else{
						// get param from the next char
						param=arg+j+1;
					}
					if(param==NULL){
						fprintf(stderr, "error: option -%c requires parameter\n", c);
						printHelp();
						exit(1);
					}
					(*opt->handler)(param);
					break; // go to the next argv.
				}else{
					(*opt->handler)(NULL);
				}
			}
		}else{
			handleInputFile(arg);
		}
	}
}



#pragma mark - Entrypoint


int main (int argc, const char * argv[])
{
	parseCommandLine(argc, argv);
	
	if(g_outputFilename.empty()){
		fprintf(stderr, "rax: error: output file name must be specified\n");
		printHelp();
		exit(1);
	}
	
	doProcess();
	
	return 0;
}