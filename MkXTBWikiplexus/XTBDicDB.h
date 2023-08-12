//
//  XTBDicDB.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"
#include <algorithm>

static const uint64_t XTBInvalidDicDBKeyAddr=0xffffffffffffffffULL;

static char charToUnsigned(char c){
	return c-128;
}

struct XTBDicDBKey{
	std::string key;
	mutable std::string unsignedKey;
	uint64_t addr;
	std::string redirection;
	bool unreachable;
	
	XTBDicDBKey(){
		unreachable=false;
		addr=XTBInvalidDicDBKeyAddr;
	}
	
	std::string& getUnsignedKey() const{
		if(key.size()!=unsignedKey.size()){
			unsignedKey=key;
			std::transform(unsignedKey.begin(), unsignedKey.end(), unsignedKey.begin(), charToUnsigned);
		}
		return unsignedKey;
	}
	
	bool operator <(const XTBDicDBKey& k) const{
		return key<k.key;
	}
	
	bool isRedirected() const{
		return !redirection.empty();
	}
};

class XTBDicDB{	
	FILE *m_handle;
	FILE *m_keysHandle;
	FILE *m_keyIndexHandle;
	
	/** need to keep this because output can be
	 * unseekable redirection. */
	uint64_t m_handlePos;
	
	std::vector<XTBDicDBKey> m_keys;
	std::map<std::string, size_t> m_indicesForKey;
	
	void resolveRedirect(size_t, int depth=0);
	void resolveRedirects();
	void removeUnreachableKeys();
	void sortKeys();
	void writeKeys() const;
	void paddingHandle(FILE *) const;
	void paddingHandle(FILE *, uint64_t&) const;
public:
	XTBDicDB(const std::string& path, bool stdoutDb=false);
	virtual ~XTBDicDB();
	
	void writeEntry(const std::string& key,
					const std::string& text);
	
	void writeRedirect(const std::string& key,
					   const std::string& newKey);
};
