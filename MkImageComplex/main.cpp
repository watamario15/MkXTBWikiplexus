//
//  main.cpp
//  MkImageComplex
//
//  Created by 河田 智明 on 8/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//


#include "stdafx.h"
#include "../MkXTBWikiplexus/XTBDicDB.h"
#include "../CSVIO/CSVIO.h"
#include "../RichgelJpeg/jpegdecoder.h"
#include <errno.h>
#include "../MkXTBWikiplexus/utils.h"

static std::string g_outputBundle;
static XTBDicDB *g_dicDB;
static FILE *g_titlesList;
static bool g_stdout=false;
//static int entryCount=0;

struct XTBSize{
	int w, h;
	XTBSize(){}
	XTBSize(int ww, int hh):
	w(ww), h(hh){}
};

#pragma mark - Jpeg Handling

static XTBSize sizeForJpegAtPath(const std::string& path){
	jpeg_decoder_file_stream stream;
	stream.open(path.c_str());
	
	jpeg_decoder decoder(&stream, false);
	if(decoder.get_error_code()){
		throw std::string("error in jpgdlib. maybe file corrupted, or not jpeg file. this file was ignored.");
	}
	
	return XTBSize(decoder.get_width(), decoder.get_height());
}

#pragma mark - Processor

static std::string escapeCSVString(const std::string& str){
	if(str.find_first_of("\"\r\n,")!=std::string::npos){
		return "\""+XTBReplace(str, "\"", "\"\"")+"\"";
	}else{
		return str;
	}
}

static std::string baseNameForPath(const std::string& path){
	std::string::size_type pos=path.find_last_of("/\\");
	if(pos==std::string::npos){
		return path;
	}else{
		return path.substr(pos+1);
	}
}

static std::string pathWithoutExtension(const std::string& path){
	std::string::size_type pos=path.find_last_of(".");
	if(pos==std::string::npos)
		return path;
	else
		return path.substr(0, pos);
}

static std::string titleForPath(const std::string& path){
	return pathWithoutExtension(baseNameForPath(path));
}

static std::string bytesForImageAtPath(const std::string& path, XTBSize size){
	std::string s;
	s+=(char)(unsigned char)((size.w>>8)&0xff);
	s+=(char)(unsigned char)((size.w)&0xff);
	s+=(char)(unsigned char)((size.h>>8)&0xff);
	s+=(char)(unsigned char)((size.h)&0xff);
	
	char buf[1024];
	FILE *f=fopen(path.c_str(), "rb");
	if(!f){
		throw "couldn't open file. ignored.";
	}
	size_t readSize=0;
	while((readSize=fread(buf, 1, 1024, f))>0){
		s.append(buf, readSize);
	}
	
	fclose(f);
	return s;
}

static void handleFile(const std::string& path){
	fprintf(stderr,"processing \"%s\"...\n", path.c_str());
	
	try{
		
		if(path.rfind(".jpg")!=path.size()-4){
			throw "filename doesn't end with \".jpg\". file ignored.";
		}
		
		std::string title=titleForPath(path);
		
		XTBSize size=sizeForJpegAtPath(path);
		
		fprintf(stderr,"info: dimension %d x %d jpeg file \"%s\"\n", size.w, size.h, title.c_str());
		
		std::string bytes=bytesForImageAtPath(path, size);
		
		g_dicDB->writeEntry(title, bytes);
		fprintf(g_titlesList, "%s\n", escapeCSVString(title).c_str());
		
	}catch(const std::string& err){
		fprintf(stderr, "error: %s: %s\n",path.c_str(), err.c_str());
		return;
	}catch(const char * err){
		fprintf(stderr, "error: %s: %s\n",path.c_str(), err);
		return;
	}
	
}

#pragma mark - Command-line

static void printHelp();

static void helpCommandLineHandler(const char *){
	printHelp();
	exit(0);
}


static void outputFilenameCommandLineHandler(const char *parameter){
	g_outputBundle=parameter;
}

static void handleInputFile(const char *fn){
	//g_inputFiles.push_back(fn);
}

static void stdoutCommandLineHandler(const char *){
	g_stdout=true;
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
	{'s', false, stdoutCommandLineHandler},
	{0, false, NULL}
};

static void printHelp(){
	puts("MkImageComplex [OPTIONS...] < LIST");
	puts("Generates ImageComplex Dictionary for XTBook.");
	puts("");
	puts("USAGE:");
	puts("");
	puts("-o OUTBUNDLE.xtbdict");
	puts("    Specifies the path for the output ImageComplex dictionary bundle.");
	puts("");
	puts("-s");
	puts("    Output Images.db to stdout for transparent compression.");
	
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
	
	if(g_outputBundle.empty()){
		fprintf(stderr, "error: output bundle must be specified\n");
		printHelp();
		exit(1);
	}
	
	if(mkdir(g_outputBundle.c_str(), 0766)){
		if(errno!=EEXIST){
			fprintf(stderr, "error: cannot create directory for output bundle\n");
			exit(2);
		}
	}

	
	g_dicDB=new XTBDicDB(g_outputBundle+"/Images");
	g_titlesList=fopen((g_outputBundle+"/Titles.txt").c_str(), "w");
	
	char buf[4096];
	
	while(gets(buf)){
		handleFile(buf);
	}
	
	fclose(g_titlesList);
	fprintf(stderr,"writing Images.db...\n");
	
	delete g_dicDB;
	
	return 0;
}