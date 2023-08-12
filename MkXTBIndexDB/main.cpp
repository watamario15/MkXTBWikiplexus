//
//  main.cpp
//  MkXTBIndexDB
//
//  Created by Kawada Tomoaki on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "stdafx.h"
#include "XTBIndexDB.h"
#include "../CSVIO/CSVIO.h"

static std::vector<std::string> g_inputFiles;
static std::string g_outputFilename;
static XTBIndexDB *g_indexDb;
static int entryCount=0;

#pragma mark - Processor

static void handleFile(const std::string& path){
	fprintf(stderr,"processing \"%s\"...\n", path.c_str());
	
	CSVIOReader reader(path.c_str());
	
	CSVIORecord record;
	
	while(record=reader.readRecord(), !record.empty()){
		if(record.size()<2){
			record.push_back(record[0]);
		}
		if(record.size()>2){
			fprintf(stderr, "warning: too many columns for record \"%s\", \"%s\", \"%s\"... some are discarded.\n", record[0].c_str(),
					record[1].c_str(), record[2].c_str());
		}
		
		g_indexDb->writeEntry(record[0], record[1]);
		
		entryCount++;
		if(entryCount%1000==0){
			fprintf(stderr, ".");
		}
		if(entryCount%10000==0){
			fprintf(stderr, "%d", entryCount);
		}
		
	}
	
	fprintf(stderr, "\n");
	
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

static void handleInputFile(const char *fn){
	g_inputFiles.push_back(fn);
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
	{0, false, NULL}
};

static void printHelp(){
	puts("MkXTBWikiplexus [OPTIONS...] INFILES");
	puts("Generates XTBook Wikiplexus dictionary for XTBook.");
	puts("");
	puts("USAGE:");
	puts("");
	puts("-o OUTBUNDLE");
	puts("    Specifies the path for the output XTBIndexDB file, without extension.");
	
	
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
	
	if(g_inputFiles.size()==0){
		fprintf(stderr, "error: no input files specified\n");
		printHelp();
		exit(1);
	}
	
	if(g_outputFilename.empty()){
		fprintf(stderr, "error: output bundle must be specified\n");
		printHelp();
		exit(1);
	}
	
	g_indexDb=new XTBIndexDB(g_outputFilename);
	
	for(size_t fileId=0;fileId<g_inputFiles.size();fileId++){
		handleFile(g_inputFiles[fileId]);
	}
	
	delete g_indexDb;
	
	return 0;
}

