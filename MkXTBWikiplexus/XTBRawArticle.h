//
//  XTBRawArticle.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"

struct XTBRawArticleIndexItem{
	int page;
	int level;
	std::string text;
	XTBRawArticleIndexItem(int _page, int _level):
	page(_page), level(_level){ /* leave text empty */}
	XTBRawArticleIndexItem(int _page, int _level, const std::string& _text):
	page(_page), level(_level), text(_text){}
	
};

class XTBRawArticle{
	std::string g_inputWiki;
	std::vector<std::string> g_lines;
	std::vector<size_t> g_pages;
	bool g_shouldPage;
	
	void explode();
	void doBraceStuff();
	std::string implodeLinesInRange(size_t beginLine,
									size_t endLine) const;
	size_t countToFitInRange(size_t beginLine,
							 size_t endLine) const;
	void doPage();
	void adjustPage();
	bool isLineHeading(size_t) const;
	std::string headingForLine(size_t) const;
	int headingLevelForLine(size_t) const;
	void removeEmptyPages();
	
public:
	XTBRawArticle(const std::string&);
	~XTBRawArticle();
	
	int pageCount() const;
	std::string wikiForPage(int) const;
	
	std::vector<XTBRawArticleIndexItem> indicesForPage(int) const;
	
};
