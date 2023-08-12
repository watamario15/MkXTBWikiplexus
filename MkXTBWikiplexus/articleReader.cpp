//
//  articleReader.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "stdafx.h"
#include "articleReader.h"
#include "utils.h"
#include <setjmp.h>

static jmp_buf g_xmlErrorCheckpoint;

static void xmlReaderErrorHandler(void *arg,
								  const char *msg,
								  xmlParserSeverities severity,
								  xmlTextReaderLocatorPtr locator){
	fprintf(stderr, "libxml error: %s\n", msg);
	fprintf(stderr, "(at line %d)\n", xmlTextReaderLocatorLineNumber(locator));
	longjmp(g_xmlErrorCheckpoint, 1);
	//exit(3);
}

void XTBReadArticles(XTBArticleReaderCallback callback, 
					 XTBArticleReaderSiteinfoCallback siteCallback,
					 int fromFd, void *param){
	
	xmlTextReaderPtr reader;
	
	reader=xmlReaderForFd(fromFd,
						  "http://www.nexhawks.net/",
						  "UTF-8",
						  0);
	
	if(reader==NULL){
		fprintf(stderr, "error: cannot create xmlTextReader\n");
		exit(3);
	}
	
	xmlTextReaderSetErrorHandler(reader, 
								 xmlReaderErrorHandler, reader);
	
	if(setjmp(g_xmlErrorCheckpoint)){
		goto closing;
	}else{
		
	}
	
	while(XTBXmlTextReaderName(reader)!="mediawiki"){
		xmlTextReaderNext(reader);
	}
	
	assert(XTBXmlTextReaderName(reader)=="mediawiki");
	xmlTextReaderRead(reader); // get in "mediawiki"
	
	while(xmlTextReaderNodeType(reader)!=15){
		
		long bytes=xmlTextReaderByteConsumed(reader);
		std::string nodeName=XTBXmlTextReaderName(reader);
		
		if(nodeName=="siteinfo"){
			(*siteCallback)(xmlTextReaderExpand(reader), param,
							bytes);
		}else if(nodeName=="page"){
			(*callback)(xmlTextReaderExpand(reader), param,
						bytes);
		}else if(nodeName=="#text"){
			// skip.
		}else{
			fprintf(stderr, "warning: unknown element \"%s\"\n",
					nodeName.c_str());
		}
		
		xmlTextReaderNext(reader);
	}
	

	
	xmlTextReaderClose(reader);
closing:;
	
}
