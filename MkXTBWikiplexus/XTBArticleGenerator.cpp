//
//  XTBArticleGenerator.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomaoki on 7/6/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "XTBArticleGenerator.h"

#include "XTBRawArticle.h"
#include "main.h"
#include "utils.h"

XTBArticleGenerator::XTBArticleGenerator(XTBRawArticle *rawArticle){
	m_rawArticle=rawArticle;
	
	
	
}

std::string XTBArticleGenerator::bytes(){
	std::string outBytes;
	
	assert(m_rawArticle->pageCount()>0);
	
	// magic number for Format1
	outBytes+=XTBBytesForSysInt32(XTBFormat1Magic);
	
	// dummy data for (uint32_t) Address to Page Table.
	outBytes+=XTBBytesForSysInt32(0);
	
	// write number of pages.
	outBytes+=XTBBytesForSysInt16(m_rawArticle->pageCount());
	
	// don't write number of indices.
	// bytesForIndex() does.
	// Because we don't know total number of indices yet.
	
	// write index.
	outBytes+=bytesForIndex();
	
	// align to 4-bytes boundary.
	alignOutBuffer(outBytes, 4);
	
	// record (uint32_t) Address to Page Table.
	outBytes.replace(4, 4, XTBBytesForSysInt32((uint32_t)outBytes.size()));
	
	// write pages.
	outBytes+=bytesForPages();
	
	return outBytes;
}


void XTBArticleGenerator::alignOutBuffer(std::string& outBytes, size_t alignment) const{
	while(outBytes.size()&(alignment-1)){
		outBytes+='\0';
	}
}

std::string XTBArticleGenerator::bytesForIndexItem(const XTBRawArticleIndexItem& item) const{
	std::string outBytes;
	outBytes+=(char)(uint8_t)(item.page+1);
	outBytes+=(char)(uint8_t)(item.level);
	outBytes+=item.text;
	outBytes+='\0';
	return outBytes;
}

std::string XTBArticleGenerator::bytesForIndex() const{
	std::string outBytes;
	std::string indices;
	const int pageCount=m_rawArticle->pageCount();
	std::vector<XTBRawArticleIndexItem> rawIndices;
	
	// collect indices...
	for(int page=0;page<pageCount;page++){
		std::vector<XTBRawArticleIndexItem> ids;
		ids=m_rawArticle->indicesForPage(page);
		
		for(size_t j=0;j<ids.size();j++)
			rawIndices.push_back(ids[j]);
	}
	
	outBytes+=XTBBytesForSysInt16(rawIndices.size());
	
	// add indices...
	std::string outIndexData;
	for(size_t i=0;i<rawIndices.size();i++){
		// register address
		outBytes+=XTBBytesForSysInt16(outIndexData.size());
		outIndexData+=bytesForIndexItem(rawIndices[i]);
	}
	
	outBytes+=outIndexData;

	return outBytes;
}

std::string XTBArticleGenerator::bytesForPages() const{
	std::string outBytes;
	std::string outPageData;
	
	const int pageCount=m_rawArticle->pageCount();
	
	for(int page=0;page<pageCount;page++){
		uint32_t offset;
		offset=(uint32_t)outPageData.size();
		outBytes+=XTBBytesForSysInt32(offset);
		
		std::string wiki=m_rawArticle->wikiForPage(page);
		outPageData+=wiki;
		outPageData+='\0';
	}
	
	outBytes+=outPageData;
	
	return outBytes;
}
