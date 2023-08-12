//
//  utils.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "utils.h"
#include "endian.h"

std::string XTBXmlTextReaderName(xmlTextReaderPtr reader){
	xmlChar *str;
	std::string str2;
	str=xmlTextReaderName(reader);
	if(str==NULL)
		return "";
	str2=(char *)str;
	free(str);
	return str2;
}

xmlNodePtr XTBChildElementWithTagName(xmlNodePtr node, xmlChar *nodeName, xmlNodePtr cont){
	if(node==NULL)
		return NULL;
	
	if(!cont)
		cont=node->children;
	else 
		cont=cont->next;
	
	while(cont){
		if(xmlNodeIsText(cont)){
			cont=cont->next;
			continue;
		}
		//printf("%s %s\n", cont->name, nodeName);
		if(!xmlStrcasecmp(cont->name, nodeName))
			break;
		cont=cont->next;
	}
	return cont;
}

std::string XTBValueForNode(xmlNodePtr parent){
	if(parent==NULL)
		return "";
	
	xmlChar *ch=xmlNodeGetContent(parent);
	std::string st=(const char *)ch;
	xmlFree(ch);
	return st;
}

std::string XTBInnerStringForNode(xmlNodePtr parent){
	std::string str;
	//printf("name: %s : %s \n", parent->name, parent->content);
	if(xmlNodeIsText(parent)){
		return (const char *)(parent->content);
	}
	
	xmlNodePtr node=parent->children;
	
	while(node){
		str+=XTBInnerStringForNode(node);
		node=node->next;
	}
	
	return str;
}

std::string XTBValueWithTagName(xmlNodePtr node, xmlChar *nodeName){
	if(node==NULL)
		return "";
	xmlNodePtr nd=XTBChildElementWithTagName(node, nodeName);
	return XTBValueForNode(nd);
}

std::string XTBAttributeForNode(xmlNodePtr parent, const xmlChar *attrName){
	if(parent==NULL)
		return "";
	if(parent->type!=XML_ELEMENT_DECL && parent->type!=XML_ELEMENT_NODE)
		return "";
	
	xmlAttributePtr attr=((xmlElementPtr)parent)->attributes;
	while(attr){
		if(!xmlStrcasecmp(attr->name, attrName)){
			
			xmlChar *chr=xmlNodeGetContent((xmlNodePtr)attr);
			if(chr){
				std::string s=(const char *)chr;
				xmlFree(chr);
				return s;
			}else{
				return "";
			}
		}
		
		attr=(xmlAttributePtr)(attr->next);
	}
	return "";
}

std::string XTBReplace(const std::string& str,
					   const std::string& from,
					   const std::string& to){
	
	std::string outString;
	std::string::size_type pos=0, nextPos;
	while(pos<str.size()){
		nextPos=str.find(from, pos);
		
		if(nextPos==std::string::npos){
			outString.append(str, pos, str.size()-pos);
			break;
		}
		
		outString.append(str, pos, nextPos-pos);
		
		outString+=to;
		
		pos=nextPos+from.size();
	}
	
	return outString;
}

std::string XTBTrimString(const std::string& str){
	std::string::size_type index1, index2;
	index1=str.find_first_not_of(L' ');
	if(index1==std::string::npos)
		return std::string();
	index2=str.find_last_not_of(L' ');
	return str.substr(index1, index2-index1+1);
}

std::string XTBNamespaceForTitle(const std::string& title){
	std::string::size_type pos;
	pos=title.find(':');
	if(pos==std::string::npos)
		return "";
	else
		return title.substr(0, pos);
}

std::string XTBPageNameForTitle(const std::string& title){
	std::string::size_type pos;
	pos=title.find(':');
	if(pos==std::string::npos)
		return title;
	else
		return title.substr(pos+1);
}

std::string XTBMakeTitle(const std::string& ns,
						 const std::string& pageName){
	if(ns.empty())
		return pageName;
	else
		return ns+':'+pageName;
}

std::string XTBSanitizeTitle(const std::string& title){
	std::string ns=XTBNamespaceForTitle(title);
	std::string pn=XTBPageNameForTitle(title);
	pn=XTBDbKeyFor(pn);
	return XTBMakeTitle(ns, pn);
}

static inline char easytolower(char in){
	if(in<='Z' && in>='A')
		return in-('Z'-'z');
	return in;
} 
static inline char easytoupper(char in){
	if(in<='z' && in>='a')
		return in-('z'-'Z');
	return in;
} 

static inline char toDbKey(char in){
	if(in==' ')
		return '_';
	return in;
} 

std::string XTBLowerStringFor(const std::string& str){
	std::string result=str;
	std::transform(result.begin(), result.end(), result.begin(), easytolower);
	return result;
}
std::string XTBUpperStringFor(const std::string& str){
	std::string result=str;
	std::transform(result.begin(), result.end(), result.begin(), easytoupper);
	return result;
}
std::string XTBDbKeyFor(const std::string& str){
	std::string out=XTBTrimString(str);
	std::transform(out.begin(), out.end(), out.begin(), toDbKey);
	if(!out.empty())
		out[0]=easytoupper(out[0]);
	return out;
}

std::vector<std::string> XTBSplit(const std::string& str,
								  const std::string& delimiter){
	std::vector<std::string> resultStrings;
	std::string::size_type i=0;
	std::string::size_type nextPos;
	while(nextPos=str.find(delimiter, i), nextPos!=std::string::npos){
		resultStrings.push_back(str.substr(i, nextPos-i));
		i=nextPos+delimiter.size();
	}
	resultStrings.push_back(str.substr(i));
	return resultStrings;
}

std::string::size_type XTBFindXMLContentEndPos(const std::string& str,
											   std::string::size_type pos){
	std::string::size_type nextPos;
	while(pos<str.size()){
		nextPos=str.find('<', pos);
		if(nextPos==std::string::npos){
			return nextPos;
		}
		
		if(nextPos<str.size()-1){
			if(str[nextPos+1]=='/'){
				// found!
				return nextPos;
			}
		}
		
		// opening new element.
		pos=nextPos+1;
		
		// get tag name for invalidation.
		// if it is an invalid tag, we can skip it.
		nextPos=str.find_first_of(" />", pos);
		if(nextPos==std::string::npos){
			// opening elemnt is not closed.
			return std::string::npos;
		}
		
		std::string tagName;
		tagName=str.substr(pos, nextPos-pos);
		pos=nextPos;
		
		nextPos=str.find('>', pos);
		if(nextPos==std::string::npos){
			// opening elemnt is not closed.
			return std::string::npos;
		}
		
		if(str[nextPos-1]=='/'){
			// instantly closed.
			pos=nextPos+1;
			continue;
		}
		
		pos=nextPos+1;
		
		if(!XTBIsTagNameValid(tagName)){
			// skip invalid tag.
			continue;
		}
		
		pos=XTBFindXMLContentEndPos(str, pos);
		
		if(pos==std::string::npos){
			// failed to close.
			return pos;
		}
		
		nextPos=str.find('>', pos+2);
		if(nextPos==std::string::npos){
			// again.. not closed.
			return std::string::npos;
		}
		pos=nextPos+1;
	}
	return std::string::npos;
}

size_t XTBCountOfSubstring(const std::string& str, 
						   const std::string& target){
	size_t count=0;
	std::string::size_type pos=0, nextPos;
	while(pos<str.size()){
		nextPos=str.find(target, pos);
		if(nextPos==std::string::npos){
			// no longer found.
			break;
		}
		count++;
		pos=nextPos+target.size();
	}
	return count;
}

bool XTBIsTagNameValid(const std::string& tagName){
	for(size_t i=1;i<tagName.size();i++){
		if((!isalnum(tagName[i])) && tagName[i]!=':')
			return false;
	}
	if(!isalpha(tagName[0]))
		return false;
	return true;
}

std::string XTBBytesForSysInt32(uint32_t value){
	union{
		uint32_t value;
		char value2[4];
	} cv;
	cv.value=XTBSysToBE32(value);
	
	return std::string(cv.value2, 4);
}

std::string XTBBytesForSysInt16(uint16_t value){
	union{
		uint16_t value;
		char value2[2];
	} cv;
	cv.value=XTBSysToBE16(value);
	
	return std::string(cv.value2, 2);
}