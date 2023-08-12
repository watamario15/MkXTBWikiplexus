//
//  XTBIndexDB.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"
#include <vector>


static char charToUnsigned(char c){
	return c-128;
}

struct XTBIndexDBEntry{
	std::string key;
	std::string title;
	
	mutable std::string unsignedKey;
	mutable std::string unsignedTitle;
	
	XTBIndexDBEntry(){}
	XTBIndexDBEntry(const std::string& k,
					const std::string& t):
	key(k), title(t){}
	
	std::string& getUnsignedKey() const{
		if(key.size()!=unsignedKey.size()){
			unsignedKey=key;
			std::transform(unsignedKey.begin(), unsignedKey.end(), unsignedKey.begin(), charToUnsigned);
		}
		return unsignedKey;
	}
	std::string& getUnsignedTitle() const{
		if(title.size()!=unsignedTitle.size()){
			unsignedTitle=title;
			std::transform(unsignedTitle.begin(), unsignedTitle.end(), unsignedTitle.begin(), charToUnsigned);
		}
		return unsignedTitle;
	}
	
	
	
	bool operator <(const XTBIndexDBEntry& entry) const{
		if(key<entry.key)
			return true;
		if(key>entry.key)
			return false;
		return title<entry.title;
	}
	bool operator !=(const XTBIndexDBEntry& entry) const{
		if(key!=entry.key)
			return true;
		if(title!=entry.title)
			return true;
		return false;
	}
};

class XTBIndexDB{
	FILE *m_handle;
	FILE *m_mapHandle;
	std::vector<XTBIndexDBEntry> m_entries;
	
	std::string bytesForEntry(size_t);
	
	void sortEntries();
	void removeDuplicated();
	void writeEntries();
	
public:
	XTBIndexDB(const std::string&);
	~XTBIndexDB();
	
	void writeEntry(const std::string& key,
					const std::string& title);
	
};
