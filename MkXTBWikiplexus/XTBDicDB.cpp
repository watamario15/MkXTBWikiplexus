//
//  XTBDicDB.cpp
//  MkXTBWikiplexus
//
//  Created by Kawda Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "XTBDicDB.h"
#include "endian.h"

XTBDicDB::XTBDicDB(const std::string& path, bool stdoutDb){
	if(stdoutDb)
		m_handle=stdout;
	else
		m_handle=fopen((path+".db").c_str(), "wb");
	m_keysHandle=fopen((path+".keys").c_str(), "wb");
	m_keyIndexHandle=fopen((path+".indices").c_str(), "wb");
	if(!m_handle){
		fprintf(stderr, "error: cannot open %s for writing",
				(path+".db").c_str());
		exit(2);
	}
	if(!m_keyIndexHandle){
		fprintf(stderr, "error: cannot open %s for writing",
				(path+".keys").c_str());
		exit(2);
	}
	if(!m_keyIndexHandle){
		fprintf(stderr, "error: cannot open %s for writing",
				(path+".indices").c_str());
		exit(2);
	}
	
	m_handlePos=0;
	
}

XTBDicDB::~XTBDicDB(){
	// we need to resolve redirections before sort because
	// sorting invalidates m_indicesForKey
	resolveRedirects();
	removeUnreachableKeys();
	sortKeys();
	writeKeys();
	if(m_handle!=stdout)
		fclose(m_handle);
	fclose(m_keysHandle);
	fclose(m_keyIndexHandle);
}

void XTBDicDB::removeUnreachableKeys(){
	
	fprintf(stderr,"removing unreachable keys...\n");
	std::vector<XTBDicDBKey> keys2;
	for(size_t i=0;i<m_keys.size();i++){
		XTBDicDBKey& k=m_keys[i];
		if(k.unreachable)
			continue;
		assert(k.addr!=XTBInvalidDicDBKeyAddr);
		keys2.push_back(k);
	}
	fprintf(stderr,"%d of %d keys remained.\n", (int)keys2.size(), (int)m_keys.size());
	keys2.swap(m_keys);
}

void XTBDicDB::sortKeys(){
	fprintf(stderr,"sorting %d keys...\n", (int)m_keys.size());
	std::sort(m_keys.begin(), m_keys.end());
}

void XTBDicDB::writeKeys() const{
	fprintf(stderr,"writing %d keys...\n", (int)m_keys.size());
	int prog1=0, prog1period=(int)m_keys.size()/200;
	int prog2=0, prog2period=prog1period*40;
	
	assert(m_keys.size()<=(size_t)UINT32_MAX);
	uint32_t keyCount=(uint32_t)m_keys.size();
	keyCount=XTBSysToBE32(keyCount);
	if(fwrite(&keyCount, 4, 1, m_keyIndexHandle)<1){
		fprintf(stderr, "error: writing number of keys failed.");
		exit(2);
	}
	
	for(size_t i=0;i<m_keys.size();i++){
		
		uint32_t value;
		
		paddingHandle(m_keysHandle);
		
		// write address
		value=(uint32_t)ftello(m_keysHandle);
		value=XTBSysToBE32(value);
		if(fwrite(&value, 4, 1, m_keyIndexHandle)<1){
			fprintf(stderr, "error: writing address of key failed.");
			exit(2);
		}
		
		
		const XTBDicDBKey& key=m_keys[i];
		
		// write lower address
		value=(uint32_t)(key.addr&0xffffffffULL);
		value=XTBSysToBE32(value);
		if(fwrite(&value, 4, 1, m_keysHandle)<1){
			fprintf(stderr, "error: writing lower address of article failed.");
			exit(2);
		}
		
		// write upper address and length
		value=(uint32_t)(key.addr>>32);
		value+=(uint32_t)(key.key.size())<<16;
		value=XTBSysToBE32(value);
		if(fwrite(&value, 4, 1, m_keysHandle)<1){
			fprintf(stderr, "error: writing lower address of article failed.");
			exit(2);
		}
		
		// write key.
		fwrite(key.key.c_str(), 1, key.key.size(), m_keysHandle);
		
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


void XTBDicDB::paddingHandle(FILE *f) const{
	off_t off=ftello(f);
	uint8_t v=0;
	while(off&0x3){
		fwrite(&v, 1, 1, f);
		off++;
	}
}

void XTBDicDB::paddingHandle(FILE *f, uint64_t& off) const{
	uint8_t v=0;
	while(off&0x3){
		fwrite(&v, 1, 1, f);
		off++;
	}
}

void XTBDicDB::writeEntry(const std::string &key, const std::string &text){
	
	paddingHandle(m_handle, m_handlePos);
	
	XTBDicDBKey dicKey;
	dicKey.key=key;
	dicKey.addr=m_handlePos;
	
	assert(text.size()<=(size_t)UINT32_MAX);
	uint32_t length=(uint32_t)text.size();
	length=XTBSysToBE32(length);
	if(fwrite(&length, 4, 1, m_handle)<1){
		fprintf(stderr, "error: writing length of an article failed.");
		exit(2);
	}
	
	m_handlePos+=4;
	
	if(text.size()>0){
		if(fwrite(text.c_str(), 1, text.size(), m_handle)<text.size()){
			fprintf(stderr, "error: writing an article failed.");
			exit(2);
		}
		m_handlePos+=text.size();
	}
	
	// fprintf(stderr,"entry \"%s\" added (%d bytes)\n",
	// 	   key.c_str(), (int)text.size());
	
	m_keys.push_back(dicKey);
	
	m_indicesForKey[dicKey.key]=m_keys.size()-1;
	
}

#pragma mark - Redirect


void XTBDicDB::writeRedirect(const std::string &key, const std::string &newKey){
	
	XTBDicDBKey dicKey;
	dicKey.key=key;
	dicKey.addr=XTBInvalidDicDBKeyAddr;
	dicKey.redirection=newKey;
	
	m_keys.push_back(dicKey);
	m_indicesForKey[dicKey.key]=m_keys.size()-1;
	
	// fprintf(stderr,"entry \"%s\" is redirect to \"%s\"\n",
	// 	   key.c_str(), newKey.c_str());
	
}


void XTBDicDB::resolveRedirects(){
	fprintf(stderr,"resolving redirects...\n");
	for(size_t i=0;i<m_keys.size();i++){
		resolveRedirect(i);
	}
}

void XTBDicDB::resolveRedirect(size_t index, int depth){
	XTBDicDBKey& key=m_keys[index];
	if(key.unreachable)
		return;
	if(key.addr!=XTBInvalidDicDBKeyAddr)
		return;
	
	assert(key.isRedirected());
	
	if(depth>32){
		// too deep redirection.
		key.unreachable=true;
		// fprintf(stderr, "warning: redirection loop detected at \"%s\".\n",
		// 		key.redirection.c_str());
		return;
	}
	
	const std::string& redirectTarget=key.redirection;
	std::map<std::string, size_t>::const_iterator it;
	
	it=m_indicesForKey.find(redirectTarget);
	
	if(it==m_indicesForKey.end()){
		// target not found.
		key.unreachable=true;
		// fprintf(stderr, "warning: redirected page \"%s\" not found.\n",
		// 		redirectTarget.c_str());
		return;
	}
	
	resolveRedirect(it->second, depth+1);
	
	XTBDicDBKey& target=m_keys[it->second];
	
	key.addr=target.addr;
	key.unreachable=target.unreachable;
	
	
}
