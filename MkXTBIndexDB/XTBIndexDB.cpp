//
//  XTBIndexDB.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "XTBIndexDB.h"
#include <algorithm>
#include "../MkXTBWikiplexus/endian.h"

XTBIndexDB::XTBIndexDB(const std::string& path){
	m_handle=fopen((path+".indexdb").c_str(), "wb");
	m_mapHandle=fopen((path+".indexmap").c_str(), "wb");
	if(!m_handle){
		fprintf(stderr, "error: cannot open %s for writing",
				(path+".indexdb").c_str());
		exit(2);
	}
	if(!m_mapHandle){
		fprintf(stderr, "error: cannot open %s for writing",
				(path+".indexmap").c_str());
		exit(2);
	}
}

XTBIndexDB::~XTBIndexDB(){
	sortEntries();
	removeDuplicated();
	writeEntries();
	fclose(m_handle);
	fclose(m_mapHandle);
}

void XTBIndexDB::writeEntries(){
	fprintf(stderr,"writing %d entries...\n", (int)m_entries.size());
	int prog1=0, prog1period=(int)m_entries.size()/200;
	int prog2=0, prog2period=prog1period*40;
	
	uint32_t entryCount=(uint32_t)m_entries.size();
	entryCount=XTBSysToBE32(entryCount);
	if(fwrite(&entryCount, 4, 1, m_mapHandle)<1){
		fprintf(stderr, "error: writing number of entries failed.");
		exit(2);
	}
	
	
	for(size_t i=0;i<m_entries.size();i++){
		
		uint32_t addr;
		addr=(uint32_t)ftello(m_handle);
		addr=XTBSysToBE32(addr);
		if(fwrite(&addr, 4, 1, m_mapHandle)<1){
			fprintf(stderr, "error: writing address to entry failed.");
			exit(2);
		}
		
		std::string entry=bytesForEntry(i);
		if(fwrite(entry.data(), entry.size(), 1, m_handle)<1){
			fprintf(stderr, "error: writing entry failed.");
			exit(2);
		}
		
		prog1++;
		prog2++;
		if(prog2>=prog2period){
			fprintf(stderr, "%d", (int)i);
			prog2=0;
		}
		if(prog1>=prog1period){
			fprintf(stderr, ".");
			prog1=0;
		}
	}
	fprintf(stderr, "\n");
}

std::string XTBIndexDB::bytesForEntry(size_t entryId){
	XTBIndexDBEntry entry=m_entries[entryId];
	std::string s;
	
	// trim to 255 bytes.
	if(entry.key.size()>255)
		entry.key.resize(255, 0);
	if(entry.title.size()>255)
		entry.title.resize(255, 0);
	
	// !: OPTIMIZATION!
	// if key==title, title is omitted, and
	// size of title becomes 0.
	if(entry.key==entry.title){
		entry.title.resize(0);
	}
	
	// write length.
	s+=(char)(unsigned char)(entry.key.size());
	s+=(char)(unsigned char)(entry.title.size());
	
	// write string.
	s+=entry.key;
	s+=entry.title;
	
	return s;
	
}

void XTBIndexDB::writeEntry(const std::string &key,
							const std::string &title){
	if(key.empty()&&title.empty()){
		// fprintf(stderr, "warning: empty entry\n");
		return;
	}
	if (key.empty()) {
		// fprintf(stderr, "warning: empty key to \"%s\"\n", title.c_str());
		return;
	}
	if (title.empty()) {
		// fprintf(stderr, "warning: \"%s\" leads to empty title\n", key.c_str());
		return;
	}
	m_entries.push_back(XTBIndexDBEntry(key, title));
}

void XTBIndexDB::sortEntries(){
	fprintf(stderr,"sorting...\n");
	std::sort(m_entries.begin(), m_entries.end());
}

void XTBIndexDB::removeDuplicated(){
	fprintf(stderr,"removing duplicates...\n");
	std::vector<XTBIndexDBEntry> newEntries;
	XTBIndexDBEntry lastEntry;
	for(size_t i=0;i<m_entries.size();i++){
		if(lastEntry!=m_entries[i]){
			newEntries.push_back(lastEntry);
			lastEntry=m_entries[i];
		}
	}
	fprintf(stderr,"%d --> %d entries.\n", (int)m_entries.size(), (int)newEntries.size());
	newEntries.swap(m_entries);
}
