//
//  CSVIO.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "CSVIO.h"


#pragma mark - Reader

CSVIOReader::CSVIOReader(const char *path){
	m_fileHandle=fopen(path, "rb");
	if(!m_fileHandle){
		throw std::string("fopen failed.");
	}
	m_shouldClose=true;
	m_buffer=new char[65536];
}

CSVIOReader::CSVIOReader(FILE *f, bool sc):
m_fileHandle(f), m_shouldClose(sc){
	m_buffer=new char[65536];
}

CSVIOReader::~CSVIOReader(){
	delete[] m_buffer;
	if(m_shouldClose)
		fclose(m_fileHandle);
}

CSVIORecord CSVIOReader::readRecord(){
	CSVIORecord record;
	std::string currentCol;
	bool inQuote=false;
	
	do{
		
		if(!fgets(m_buffer, 65535, m_fileHandle)){
			return record;
		}
		
		for(const char *ptr=m_buffer;*ptr;ptr++){
			if(inQuote){
				if(*ptr=='"'){
					// end of quote?
					if(*(ptr+1)=='"'){
						// no, escaped quote.
						currentCol+='"';
						ptr++;
						continue;
					}
					inQuote=false;
				}else if(*ptr == '\n' || *ptr == '\r'){
					fprintf(stderr, "Encountered a quote error, YomiGenesis might be broken.\n");
					inQuote = false;
					continue;
				}else{
					currentCol+=*ptr;
				}
			}else{
				if(*ptr=='"'){
					// begin of quote.
					inQuote=true;
				}else if(*ptr==','){
					// end of col.
					record.push_back(currentCol);
					currentCol.clear();
				}else if(*ptr=='\n' || *ptr=='\r'){
					continue;
				}else{
					currentCol+=*ptr;
				}
			}
		}
		
		
		
	}while(inQuote);
	record.push_back(currentCol);
	return record;
}

#pragma mark - Writer

CSVIOWriter::CSVIOWriter(const char *path){
	m_fileHandle=fopen(path, "wb");
	if(!m_fileHandle){
		throw std::string("fopen failed.");
	}
	m_shouldClose=true;
}

CSVIOWriter::CSVIOWriter(FILE *f, bool sc):
m_fileHandle(f), m_shouldClose(sc){
}

CSVIOWriter::~CSVIOWriter(){
	if(m_shouldClose)
		fclose(m_fileHandle);
}

std::string CSVIOWriter::escape(const std::string &s){
	if(s.find_first_of("\"\r\n,")!=std::string::npos){
		std::string outStr;
		outStr+='"';
		for(size_t i=0;i<s.size();i++){
			if(s[i]=='"')
				outStr+="\"\"";
			else
				outStr+=s[i];
		}
		outStr+='"';
		return outStr;
	}else{
		// no need
		return s;
	}
}

void CSVIOWriter::writeRecord(const CSVIORecord &record){
	std::string records;
	for(size_t i=0;i<record.size();i++){
		if(i>0)
			records+=',';
		records+=this->escape(record[i]);
	}
	fprintf(m_fileHandle, "%s\n", records.c_str());
}
