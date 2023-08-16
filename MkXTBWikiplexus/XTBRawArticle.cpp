//
//  XTBRawArticle.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "XTBRawArticle.h"
#include "main.h"
#include "utils.h"

const static char g_lineEnd='\n';

XTBRawArticle::XTBRawArticle(const std::string& wiki){
	g_inputWiki=wiki;
	g_shouldPage=(wiki.size()>g_pageThreshold);
	
	explode();
	
	// size_t firstLines=g_lines.size();
	doBraceStuff();
	
	// fprintf(stderr,"clustered liness from %d to %d lines\n",
	// 	   (int)firstLines, (int)g_lines.size());
	
	if(g_shouldPage){
		
		
		doPage();
		
		removeEmptyPages();
		
		// fprintf(stderr,"%d page(s) generated.\n",
		// 	   (int)pageCount());
		
		adjustPage();
	}
}

XTBRawArticle::~XTBRawArticle(){
	
}

#pragma mark - Splitter

void XTBRawArticle::explode(){
	std::string::size_type pos=0, nextPos, pos2,
	lastPos=0;;
	
	std::string search="<";
	search+=g_lineEnd;
	while(pos<g_inputWiki.size()){
		nextPos=g_inputWiki.find_first_of(search, pos);
		if(nextPos==std::string::npos){
			g_lines.push_back(g_inputWiki.substr(lastPos));
			lastPos=g_inputWiki.size();
			break;
		}
		
		char found=g_inputWiki[nextPos];
		
		if(found=='<'){
			
			// begin html tag.
			
			pos2=nextPos;
			
			if(g_inputWiki[nextPos+1]=='/'){
				// element is not closed.
				pos=nextPos+1;
				continue;
			}
			
			// first, find tag name.
			pos2=g_inputWiki.find_first_of(" />", pos2);
			
			if(pos2==std::string::npos){
				// element is not closed.
				pos=nextPos+1;
				continue;
			}
			
			std::string tagName;
			tagName=g_inputWiki.substr(nextPos+1, pos2-nextPos-1);
			
			if(!XTBIsTagNameValid(tagName)){
				// invalid tag.
				// just pass through.
				/*
				fprintf(stderr, "info: explode(): invalid tag \"%s\" was ignored.", tagName.c_str());*/
			invalid:
				pos=nextPos+1;
				continue;
			}
			
			
			pos2=g_inputWiki.find('>', pos2);
			
			if(pos2==std::string::npos){
				// bug fixed: crash!
				goto invalid;
			}
			
			if(g_inputWiki[pos2-1]=='/'){
				// instantly closed.
				pos=pos2+1;
				continue;
			}
			
			pos=pos2+1;
			if(tagName=="pre"){
				pos=g_inputWiki.find("</pre>", pos);
			}else if(tagName=="nowiki"){
				pos=g_inputWiki.find("</nowiki>", pos);
			}else if(tagName=="code"){
				pos=g_inputWiki.find("</code>", pos);
			}else if(tagName=="gallery"){
				pos=g_inputWiki.find("</gallery>", pos);
			}else{
				pos=XTBFindXMLContentEndPos(g_inputWiki, pos);
			}
			
			if(pos==std::string::npos){
				goto invalid;
			}
			
			assert(g_inputWiki[pos]=='<');
			assert(g_inputWiki[pos+1]=='/');
			
			
			pos2=g_inputWiki.find('>', pos);
			if(pos2==std::string::npos){
				// element is not closed.
				g_lines.push_back(g_inputWiki.substr(lastPos));
				// fprintf(stderr, "warning: element invalidly closed\n");
				lastPos=g_inputWiki.size();
				break;
			}
			
			pos=pos2+1;
			
		}else if(found==g_lineEnd){
			g_lines.push_back(g_inputWiki.substr(lastPos, nextPos-lastPos));
			
			pos=lastPos=nextPos+1;
		}
		
	}
	
	g_lines.push_back(g_inputWiki.substr(lastPos));
	
	assert(g_lines.size()>0);
}

void XTBRawArticle::doBraceStuff(){
	std::vector<std::string> newLines;
	size_t i=0;
	while(i<g_lines.size()){
		
		const std::string& line=g_lines[i];
		std::string outLine;
		
		size_t openCount;
		size_t closeCount;
		
		openCount=XTBCountOfSubstring(line, "{");
		closeCount=XTBCountOfSubstring(line, "}");
		
		if(openCount>closeCount){
			// begin opened something.
			
			outLine=line;
			
			i++;
			
			while(i<g_lines.size()){
				
				const std::string& line2=g_lines[i];
				outLine+=g_lineEnd;
				outLine+=line2;
				
				i++;
				
				openCount+=XTBCountOfSubstring(line2, "{");
				closeCount+=XTBCountOfSubstring(line2, "}");
				
				/*
				if(closeCount>openCount){
					fprintf(stderr, "warning: brace was closed too much (%d>%d)\n",
							(int)closeCount, (int)openCount);
				}
				*/

				if(openCount<=closeCount)
					break;
				
			}
			
			/*
			if(closeCount<openCount){
				fprintf(stderr, "warning: brace was not closed much enough (%d<%d)\n",
						(int)closeCount, (int)openCount);
				
			}
			*/

			newLines.push_back(outLine);
			
		}else{
			// closed instantly, or error.
			// just ignore it.
			
			newLines.push_back(line);
			
			/*
			// report error if there is one.
			if(closeCount>openCount){
				fprintf(stderr, "warning: brace was closed too much (%d>%d)\n",
						(int)closeCount, (int)openCount);
			}
			*/

			i++;
		}
	}
	
	newLines.swap(g_lines);
}

std::string XTBRawArticle::implodeLinesInRange(size_t beginLine,
								size_t endLine) const{
	std::string outString;
	for(size_t i=beginLine;i<endLine;i++){
		if(i>0)
			outString+=g_lineEnd;
		outString+=g_lines[i];
	}
	return outString;
}

size_t XTBRawArticle::countToFitInRange(size_t beginLine,
						 size_t endLine) const{
	size_t line=beginLine;
	size_t totalSize=0;
	while(line<endLine){
		totalSize+=g_lines[line].size();
		if(line>beginLine)
			totalSize++; // line-break.
		
		if(totalSize>g_pageSplitSize)
			break;
		
		line++;
	}
	return line-beginLine;
}

void XTBRawArticle::doPage(){
	size_t lastLine=0;
	g_pages.push_back(0);
	
	while(lastLine<g_lines.size()){
		size_t count;
		count=countToFitInRange(lastLine, g_lines.size());
		if(count==0){
			count=1; // blank page? not!
			// fprintf(stderr, "page %d with %d bytes exceeds limit %d bytes\n",
			// 		(int)g_pages.size(), (int)g_lines[lastLine].size(), (int)g_pageSplitSize);
			
			//puts(g_lines[lastLine].c_str());
			//abort();
		}
		
		lastLine+=count;
		g_pages.push_back(lastLine);
	}
}

void XTBRawArticle::removeEmptyPages(){
	for(size_t i=0;i<g_pages.size()-1;i++){
		std::string wiki=wikiForPage((int)i);
		if(wiki.find_first_not_of(" \n\t\r")==std::string::npos){
			// empty.
			if(i<g_pages.size()-2){
				// bring next page's first border to here.
				g_pages[i+1]=g_pages[i];
				g_pages.erase(g_pages.begin()+i);
				i--;
			}else{
				// begin previous page's end border to here.
				g_pages.erase(g_pages.begin()+i);
				i--;
			}
		}
	}
}

void XTBRawArticle::adjustPage(){
	// avoid heading coming to the end of a page.
	for(size_t i=1;i<g_pages.size()-1;i++){
		size_t lineIndex=g_pages[i];
		
		if(isLineHeading(lineIndex-1)){
			// adjust page border.
			g_pages[i]--;
		}
	}
}

bool XTBRawArticle::isLineHeading(size_t line) const{
	assert(line>=0);
	assert(line<g_lines.size());
	const std::string& l=g_lines[line];
	if(l.empty())
		return false;
	if(l[0]=='=')
		return true;
	else
		return false;
}

std::string XTBRawArticle::headingForLine(size_t line) const{
	const std::string& l=g_lines[line];
	std::string::size_type pos;
	pos=l.find_first_not_of('=');
	
	if(l.find('<')!=std::string::npos){
		// containing html tags.
		// we'd better not to do with this heading.
		// that's surprising.
		return "";
	}
	
	std::string heading=l.substr(pos, l.size()-pos*2);
	heading=XTBTrimString(heading);
	return heading;
}

int XTBRawArticle::headingLevelForLine(size_t line) const{
	const std::string& l=g_lines[line];
	std::string::size_type pos;
	pos=l.find_first_not_of('=');
	if(pos<2)
		pos=2;
	return (int)pos-2;
}

#pragma mark - Front-end

int XTBRawArticle::pageCount() const{
	if(g_shouldPage)
		return (int)g_pages.size()-1;
	else
		return 1;
}

std::string XTBRawArticle::wikiForPage(int page) const{
	if(g_shouldPage){
		size_t beginLine=g_pages[page];
		size_t endLine=g_pages[page+1];
		return implodeLinesInRange(beginLine, endLine);
	}else{
		return g_inputWiki;
	}
}

std::vector<XTBRawArticleIndexItem> XTBRawArticle::indicesForPage(int page) const{
	
	std::vector<XTBRawArticleIndexItem> indices;
	
	assert(page>=0);
	assert(page<pageCount());
	
	size_t beginLine;
	size_t endLine;
	if(g_shouldPage){
		beginLine=g_pages[page];
		endLine=g_pages[page+1];
	}else{
		beginLine=0;
		endLine=g_lines.size();
	}
	
	if(g_lines.empty()){
		// fprintf(stderr, "warning: indicesForPage: empty\n");
		return indices;
	}
	
	// add page header index, if there is no heading
	// in the first line.
	
	if(!isLineHeading(beginLine)){
		indices.push_back(XTBRawArticleIndexItem(page, 0));
	}
	
	// scan and add indices
	for(size_t line=beginLine;line<endLine;line++){
		if(isLineHeading(line)){
			
			std::string heading=headingForLine(line);
			if(heading.empty()){
				// empty. don't deal with this.
				continue;
			}
			
			int level=headingLevelForLine(line);
			
			indices.push_back(XTBRawArticleIndexItem(page, level, heading));
			
		}
	}
	
	return indices;
}



