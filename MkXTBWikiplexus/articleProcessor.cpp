//
//  articleProcessor.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "articleProcessor.h"
#include "XTBDicDB.h"
#include "main.h"
#include "utils.h"
#include "siteInfo.h"
#include "XTBRawArticle.h"
#include "XTBArticleGenerator.h"
#include <exception>

static void registerArticlePage(const std::string& title,
						   const std::string& text,
						   XTBDicDB *db){
	XTBRawArticle article(text);
	

	
	XTBArticleGenerator articleGen(&article);
	std::string outBytes=articleGen.bytes();
	
	db->writeEntry(title, outBytes);
}

static void registerTemplatePage(const std::string& title,
								 const std::string& text,
								 XTBDicDB *db){
	db->writeEntry(title, text);
}

static void registerRedirection(const std::string& title,
								const std::string& target,
								XTBDicDB *db){
	db->writeRedirect(title, target);
}

#pragma mark - Preprocessing

static std::string preprocessCleanup(std::string str){
	str=XTBReplace(str, "<includeonly>", "");
	str=XTBReplace(str, "</includeonly>", "");
	str=XTBReplace(str, "<noinclude>", "");
	str=XTBReplace(str, "</noinclude>", "");
	str=XTBReplace(str, "<onlyinclude>", "");
	str=XTBReplace(str, "</onlyinclude>", "");
	return str;
}

static std::string handleOnlyIncludeForInclusion(const std::string& str){
	std::string outString;
	if(str.find("<onlyinclude>")==std::string::npos){
		return str; // no <onlyinclude>
	}
	
	std::string::size_type pos=0, newPos;
	while(pos<str.size()){
		newPos=str.find("<onlyinclude>", pos);
		if(newPos==std::string::npos){
			break;
		}
		
		pos=newPos+13;
		
		newPos=str.find("</onlyinclude>", pos);
		if(newPos==std::string::npos){
			newPos=str.size();
		}
		
		outString.append(str, pos, newPos-pos);
		
		pos=newPos+14;
	}
	
	return outString;
}

static std::string ignoreElement(const std::string& str,
								 const std::string& element){
	std::string::size_type pos=0, newPos;
	std::string outString;
	std::string openTag="<"+element;
	std::string closeTag="</"+element;
	
	while(pos<str.size()){
		newPos=str.find(openTag, pos);
		if(newPos==std::string::npos){
			outString.append(str, pos, str.size()-pos);
			break;
		}
		
		outString.append(str, pos, newPos-pos);
		
		pos=newPos;
		
		std::string::size_type namePos=pos+1;
		
		newPos=str.find(closeTag, pos);
		if(newPos==std::string::npos){
			// not closed.
			outString+=L'<';
			pos++;
			continue;
		}
		
		pos=newPos;
		
		newPos=str.find('>', pos);
		if(newPos==std::string::npos){
			// not closed.
			outString+=L'<';
			pos=namePos;
			continue;
		}
		
		pos=newPos+1;
	}
	
	return outString;
}

static std::string preprocessForInclusion(const std::string& str){
	std::string s=str;
	s=handleOnlyIncludeForInclusion(str);
	s=ignoreElement(s, "noinclude");
	s=preprocessCleanup(s);
	return s;
}

static std::string preprocessForArticle(const std::string& str){
	std::string s=str;
	s=ignoreElement(s, "includeonly");
	s=preprocessCleanup(s);
	return s;
}

static std::string removeComments(const std::string& str){
	std::string::size_type pos=0, newPos;
	std::string outString;
	while(pos<str.size()){
		newPos=str.find("<!--", pos);
		if(newPos==std::string::npos){
			outString.append(str, pos, str.size()-pos);
			break;
		}
		
		outString.append(str, pos, newPos-pos);
		
		pos=newPos;
		
		newPos=str.find("-->", pos);
		if(newPos==std::string::npos){
			// not closed.
			break;
		}
		
		pos=newPos+3;
	}
	
	return outString;
}

#pragma mark - Redirections

static inline wchar_t easytoupper(wchar_t in){
	if(in<=L'z' && in>=L'a')
		return in-(L'z'-L'Z');
	return in;
} 


static bool isTextRedirect(const std::string& text){
	if(text[0]!='#')
		return false;
	
	std::string text2=text.substr(0, 100);
	std::transform(text2.begin(), text2.end(), text2.begin(), easytoupper);
	if(text2.find("#REDIRECT")==0)
		return true;
	
	if(text.size()<128 && text.find("[[")!=std::string::npos &&
	   text.find("]]", text.find("[["))!=std::string::npos &&
	   text.find("#", text.find("]]"))==std::string::npos &&
	   text.find("{")==std::string::npos){
		// may be redirect...!
		return true;
	}
	
	return false;
}

static std::string redirectTargetForText(const std::string& text){
	if(!isTextRedirect(text))
		return ""; // oops?
	
	std::string::size_type pos, newPos;
	pos=text.find("[[");
	if(pos==std::string::npos){
		// there is no link; invalid redirection.
		return "";
	}
	
	pos+=2;
	
	newPos=text.find("]]", pos);
	if(newPos==std::string::npos){
		// assume link is closed in end of text
		newPos=text.size();
	}
	
	assert(pos<=newPos);
	
	std::string target=text.substr(pos, newPos-pos);
	
	if(target.find('#')!=std::string::npos){
		// ignore section link
		target=target.substr(0, target.find('#'));
	}
	return target;
}

#pragma mark - HTML to XHTML


static size_t sanitizeElement(const std::string& html,
						   std::string& outHtml,
						   size_t pos,
							  bool isRoot){
	size_t nextPos;
	std::string lastTag;

	while(pos<html.size()){
		nextPos=html.find('<', pos);
		
		if(nextPos==std::string::npos){
			outHtml.append(html, pos, html.size()-pos);
			return html.size();
		}
		
		if(html[nextPos+1]=='/'){
			// end of some tag.
			outHtml.append(html, pos, nextPos-pos);
			pos=nextPos+2;
			nextPos=html.find('>', pos);
			
			if(nextPos==std::string::npos || isRoot){
				outHtml+="</";
				continue;
			}
			
			std::string tagName;
			tagName=html.substr(pos, nextPos-pos);
			
			bool containingTagClosed=false;
			
			if(lastTag==tagName) // closing lastTag.
				lastTag.clear(); 
			else{ // closing conatining tag.
				// if unclosed tag remains, close it.
				containingTagClosed=true;
				if(!lastTag.empty()){
					outHtml+="</";
					outHtml+=lastTag;
					outHtml+=">";
				}
			}
			
			outHtml+="</";
			
			nextPos++;
			outHtml.append(html, pos, nextPos-pos);
			
			pos=nextPos;
			
			if(containingTagClosed){
				
				return pos;
			}
			
			continue;
		}else{
			
			// begin of some tag.
			outHtml.append(html, pos, nextPos-pos);
			
			pos=nextPos+1;
			
			// extract the tag name.
			nextPos=html.find_first_of(" />", pos);
			
			if(nextPos==std::string::npos){
				outHtml+="<";
				continue;
			}
			
			std::string tagName;
			tagName=html.substr(pos, nextPos-pos);
			
			bool shouldSanitize=true;
			bool shouldInstantlyClose=false;
			
			if(tagName=="li" || tagName=="dt" || tagName=="dd"){
				// if unclosed tag remains, close it.
				if(!lastTag.empty()){
					outHtml+="</";
					outHtml+=lastTag;
					outHtml+=">";
				}
				lastTag=tagName;
				shouldSanitize=false;
			}else if(tagName=="pre"){
				// pre. ignore while </pre> is found.
				nextPos=html.find("</pre>", nextPos);
				if(nextPos==std::string::npos){
					fprintf(stderr, "warning: sanitizeList: </pre> not found.\n");
					nextPos=html.size();
				}
				outHtml+='<';
				outHtml.append(html, pos, nextPos-pos);
				pos=nextPos;
				continue;
			}else if(tagName=="br"){
				shouldInstantlyClose=true;
			}else if(tagName=="hr"){
				shouldInstantlyClose=true;
			}else if(!XTBIsTagNameValid(tagName)){
				// skip this invalid tag.
				outHtml+='<';
				continue;
			}
			/* NOTE: <nowiki> doesn't disable any HTML tags.
			 *       
			 */
			
			//printf("OPENED %s\n", tagName.c_str());
			
			outHtml+='<';
			
			nextPos=html.find('>', nextPos);
			if(nextPos==std::string::npos){
				fprintf(stderr, "warning: sanitizeList: opening tag not closed.\n");
				continue;
			}
			
			nextPos++;
			if(shouldInstantlyClose){
				if(html[nextPos-2]!='/'){
					// insert "/>" to sanitize.
					outHtml.append(html, pos, nextPos-pos-1);
					outHtml+="/>";
					pos=nextPos;
					continue;
				}
			}
			outHtml.append(html, pos, nextPos-pos);
			
			pos=nextPos;
			
			if(html[nextPos-2]=='/'){
				// instantly closed.
				continue;
			}
			
			if(shouldSanitize)
			pos=sanitizeElement(html, outHtml, pos, false);
			
		}
		
	}
	return pos;
}

static std::string resolveClearlyInvalidLt(const std::string& html){
	std::string txt;
	std::string::size_type pos=0, nextPos;
	while(pos<html.size()){
		nextPos=html.find('<', pos);
		
		if(nextPos==std::string::npos){
			txt.append(html, pos, html.size()-pos);
			break;
		}
		
		txt.append(html, pos, nextPos-pos);
		
		pos=nextPos+1;
		
		if((!isalpha(html[pos])) && html[pos]!='/' && html[pos]!='{'){
			// invalid tag name.
			txt+="&lt;";
			continue;
		}
		
		// correct.
		txt+='<';
	}
	return txt;
}

static std::string sanitizeHTML(const std::string& html){
	
	std::string txt=XTBTrimString(html);
	txt=removeComments(txt);
	//printf("sanitization original --> %d bytes\n", (int)txt.size());
	txt=XTBReplace(txt, "\r\n", "\n");
	txt=XTBReplace(txt, "\r", "\n");
	txt=XTBReplace(txt, "<br>", "<br/>");
	txt=XTBReplace(txt, "<br />", "<br/>");
	
	//printf("sanitization line breaks --> %d bytes\n", (int)txt.size());
	
	txt=resolveClearlyInvalidLt(txt);
	
	//printf("sanitization resolveClearlyInvalidLt --> %d bytes\n", (int)txt.size());
	
	/* sanitization is no longer needed. */
	return txt;
	std::string txt2;
	std::string::size_type pos;
	pos=sanitizeElement(txt, txt2, 0, true);
	if(pos<txt.size()){
		fprintf(stderr, "warning: sanitizing strange!\n");
	}
	txt2.append(txt, pos, txt.size()-pos);
	
	
	//printf("sanitization lists --> %d bytes\n", (int)txt.size());
	
	return txt2;
}

#pragma mark - CSV

static std::string escapeCSVString(const std::string& str){
	if(str.find_first_of("\"\r\n,")!=std::string::npos){
		return "\""+XTBReplace(str, "\"", "\"\"")+"\"";
	}else{
		return str;
	}
}

static std::string recordForTitles(const std::string& title,
								   const std::string& redirect){
	if(title==redirect)
		return escapeCSVString(title)+"\n";
	else
		return escapeCSVString(title)+","+escapeCSVString(redirect)+"\n";
}

static std::string recordForBaseNames(const std::string& title,
									  const std::string& redirect){
	std::string baseName;
	size_t pos=title.find(':');
	if(pos==std::string::npos){
		baseName=title;
	}else{
		// is this really namespace?
		std::string nsName=title.substr(0, pos);
		if(XTBDoesNamespaceExists(nsName))
			baseName=title.substr(pos+1);
		else
			baseName=title;
	}
	return escapeCSVString(baseName)+","+escapeCSVString(redirect)+"\n";
}

#pragma mark - Main

void XTBProcessArticle(const std::string& title,
					   const std::string& text,
					   XTBDicDB *articleDb,
					   XTBDicDB *templateDb,
					   FILE *titlesList,
					   FILE *baseNamesList){
	
	try{
		std::string txt=sanitizeHTML(text);
		
		std::string g_templatePrefix=XTBNameForStandardNamespace(TWStandardNamespaceTemplate)+':';
		
		if(txt.empty()){
			fprintf(stderr,"article %s was skipped because it's empty\n",
				   title.c_str());
		}
		
		bool isRedirect=isTextRedirect(text);
		std::string redirectTarget=redirectTargetForText(text);
		
		if(isRedirect && redirectTarget.empty()){
			fprintf(stderr, "warning: invalid redirection in \"%s\". ignoring.\n",
					title.c_str());
			return;
		}
		
		redirectTarget=XTBSanitizeTitle(redirectTarget);
		
		if(title.find(g_templatePrefix)==0){
			// this is a template.
			
			if(g_emitTemplatePage){
				if(isRedirect){
					registerRedirection(title, redirectTarget, 
										articleDb);
				}else{
					std::string preprocessedWiki;
					preprocessedWiki=preprocessForArticle(txt);
					
					registerArticlePage(title, preprocessedWiki,
										articleDb);
				}
			}
			
			if(isRedirect){
				registerRedirection(XTBPageNameForTitle(title), 
									XTBPageNameForTitle(redirectTarget), 
									templateDb);
			}else{
				std::string preprocessedWiki, templateName;
				preprocessedWiki=preprocessForInclusion(txt);
				
				if(preprocessedWiki.size()>g_templateSizeLimit){
					fprintf(stderr,"template %s was skipped because its size(%d) exceeds limit(%d)\n",
						   title.c_str(), (int)preprocessedWiki.size(),
						   (int)g_templateSizeLimit);
					return;
				}
				
				templateName=XTBPageNameForTitle(title);
				registerTemplatePage(templateName, preprocessedWiki,
									 templateDb);
			}
			
		}else{
			
			// normal article page.
			
			if(isRedirect){
				fprintf(titlesList, "%s",
						recordForTitles(title, redirectTarget).c_str());
				fprintf(baseNamesList, "%s",
						recordForBaseNames(title, redirectTarget).c_str());
				
				registerRedirection(title, 
									redirectTarget, 
									articleDb);
			}else{
				fprintf(titlesList, "%s",
						recordForTitles(title, title).c_str());
				fprintf(baseNamesList, "%s",
						recordForBaseNames(title, title).c_str());
				
				
				std::string preprocessedWiki;
				preprocessedWiki=preprocessForArticle(txt);
				
				registerArticlePage(title, preprocessedWiki,
									articleDb);
			}
			
		}
		
	}catch(const std::string& e){
		fprintf(stderr, "warning: exception \"%s\" was thrown while"
				" processing \"%s\", maybe skipped.\n",
				e.c_str() ,title.c_str());
	}catch(const char * e){
		fprintf(stderr, "warning: exception \"%s\" was thrown while"
				" processing \"%s\", maybe skipped.\n",
				e ,title.c_str());
	}catch(const std::exception& e){
		fprintf(stderr, "warning: exception \"%s\" was thrown while"
				" processing \"%s\", maybe skipped.\n",
				e.what() ,title.c_str());
	}
	
	
}

