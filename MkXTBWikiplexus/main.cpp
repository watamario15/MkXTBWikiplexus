//
//  main.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "stdafx.h"
#include "articleReader.h"
#include "utils.h"
#include <errno.h>
#include "articleProcessor.h"
#include "XTBDicDB.h"
#include "siteInfo.h"
#include <ctype.h>

std::vector<std::string> g_inputFiles;
std::string g_outputBundle;

bool g_emitTemplatePage=true;
bool g_stdout=false;

size_t g_pageThreshold=13384;
size_t g_pageSplitSize=10000;
size_t g_pageCountLimit=254;
size_t g_templateSizeLimit=17768;

static XTBDicDB *g_articleDb;
static XTBDicDB *g_templateDb;
static FILE *g_titlesList;
static FILE *g_baseNamesList;
static std::set<int> g_excludedStdNamespaces;

#pragma mark - Path

static std::string pathForArticle(){
	return g_outputBundle+"/Articles";
}

static std::string pathForTemplate(){
	return g_outputBundle+"/Templates";
}

static std::string pathForSiteInfo(){
	return g_outputBundle+"/SiteInfo.plist";
}

static std::string pathForTitles(){
	return g_outputBundle+"/Titles.csv";
}

static std::string pathForBaseNames(){
	return g_outputBundle+"/BaseNames.csv";
}

#pragma mark - Reader

static unsigned long g_oldBytes=0;
static unsigned long g_wrapCount=0;

static void handleArticle(xmlNodePtr node, void *, long bytes){
	std::string title;
	title=XTBValueWithTagName(node, BAD_CAST "title");
	title=XTBSanitizeTitle(title);
	
	// is this namespace excluded?

	std::string ns=XTBNamespaceForTitle(title);
	if(XTBIsNamespaceExcluded(ns)){
		fprintf(stderr,"namespace of %s was excluded\n",
			   title.c_str());
		return;
	}
	
	
	// calculate current position. 

	unsigned long bytes2=bytes;
	if(bytes2<g_oldBytes){
		// wrapped because of limit of long
		g_wrapCount++;
	}
	g_oldBytes=bytes2;
	
	unsigned long long realBytes;
	realBytes=bytes2;
	realBytes+=0x100000000ULL*g_wrapCount;
	
	
	// retrive the text.
	
	std::string text;
	
	xmlNodePtr revision=XTBChildElementWithTagName(node, 
												   BAD_CAST "revision");
	
	if(!revision){
		fprintf(stderr, "article %s doesn't have <revision>\n",
				title.c_str());
		return;
	}
	
	xmlNodePtr textNode=XTBChildElementWithTagName(revision, BAD_CAST "text");
	text=XTBInnerStringForNode(textNode);
	
	
	// output log.
	
	fprintf(stderr,"converting \"%s\" (%d bytes) at offset %llu (0x%llx)\n",
		   title.c_str(), (int)text.size(), realBytes, realBytes);
	
	fprintf(stderr, "   [%s]\n", title.c_str());
	
	XTBProcessArticle(title, text, 
					  g_articleDb, g_templateDb,
					  g_titlesList, g_baseNamesList);
}

static void handleSiteinfo(xmlNodePtr node, void *, long bytes){
	XTBExpandSiteInfo(node);
	XTBWriteSiteInfo(pathForSiteInfo());
	
	// exclude standard naemspaces
	for(std::set<int>::iterator it=
		g_excludedStdNamespaces.begin();
		it!=g_excludedStdNamespaces.end();
		it++){
		try{
		XTBExcludeNamespace(XTBNameForStandardNamespace(*it));
		}catch(...){}
	}
}

#pragma mark - Command-line

static void printHelp();

static void helpCommandLineHandler(const char *){
	printHelp();
	exit(0);
}

static void excludeNamesapceCommandLineHandler(const char *parameter){
	if(parameter[0]=='-' || isdigit(parameter[0])){
		g_excludedStdNamespaces.insert(atoi(parameter));
	}else{
		XTBExcludeNamespace(parameter);
	}
}

static void outputFilenameCommandLineHandler(const char *parameter){
	g_outputBundle=parameter;
}

static void noTemplatePageCommandLineHandler(const char *){
	g_emitTemplatePage=false;
}

static void stdoutDbCommandLineHandler(const char *){
	g_stdout=true;
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
	{'m', false, noTemplatePageCommandLineHandler},
	{'x', true, excludeNamesapceCommandLineHandler},
	{'s', false, stdoutDbCommandLineHandler},
	{0, false, NULL}
};

static void printHelp(){
	puts("MkXTBWikiplexus [OPTIONS...] [<] INFILE");
	puts("Generates XTBook Wikiplexus dictionary for XTBook.");
	puts("");
	puts("USAGE:");
	puts("");
	puts("-o OUTBUNDLE");
	puts("    Specifies the path for the output XTBook Dictionary bundle.");
	puts("-s");
	puts("    Output Articles.db to stdout for transparent compression.");
	puts("-m");
	puts("    Prevents to emit template pages.");
	puts("-x {NAMESPACE-ID|NAMESPACE-NAME}");
	puts("    Excludes specified namespace.");
	puts("");
	puts("NAMESPACE-IDs:");
	puts(" -2 Media");
	puts(" -1 Special");
	puts(" 0 Main");
	puts(" 1 Talk");
	puts(" 2 User");
	puts(" 3 User Talk");
	puts(" 4 Project");
	puts(" 5 Project Talk");
	puts(" 6 File");
	puts(" 7 File Talk");
	puts(" 8 MediaWiki");
	puts(" 9 MediaWiki Talk");
	puts(" 10 Template");
	puts(" 11 Template Talk");
	puts(" 12 Help");
	puts(" 13 Help Talk");
	puts(" 14 Category");
	puts(" 15 Category Talk");
	
	
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
	
	if(g_inputFiles.size()==1){
		if(!freopen(g_inputFiles[0].c_str(), "r", stdin)){
			fprintf(stderr, "error: cannot open %s for reading\n", g_inputFiles[0].c_str());
			exit(2);
		}
	}else if(g_inputFiles.size()>1){
		fprintf(stderr, "error: too many input files specified\n");
		printHelp();
		exit(1);
	}
	
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
	
	g_articleDb=new XTBDicDB(pathForArticle(), g_stdout);
	g_templateDb=new XTBDicDB(pathForTemplate());
	g_titlesList=fopen(pathForTitles().c_str(), "wb");
	g_baseNamesList=fopen(pathForBaseNames().c_str(), "wb");
	
	XTBReadArticles(handleArticle,
					handleSiteinfo,
					0, NULL);
	
	fprintf(stderr,"finalizing Articles.db...\n");
	delete g_articleDb;
	
	fprintf(stderr,"finalizing Templates.db...\n");
	delete g_templateDb;
	
	fclose(g_titlesList);
	fclose(g_baseNamesList);
	
    return 0;
}

