//
//  XTBArticleGenerator.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/6/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once


#include "stdafx.h"

class XTBRawArticle;
struct XTBRawArticleIndexItem;

const uint32_t XTBFormat1Magic=0xf1c0ffee;

class XTBArticleGenerator{

	XTBRawArticle *m_rawArticle;
	
	std::string bytesForIndex() const;
	
	std::string bytesForPages() const;
	
	void alignOutBuffer(std::string&, size_t) const;
	
	std::string bytesForIndexItem(const XTBRawArticleIndexItem&) const;
	
public:
	
	XTBArticleGenerator(XTBRawArticle *);
	
	std::string bytes();
	
};


