//
//  utils.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include <string>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

std::string XTBXmlTextReaderName(xmlTextReaderPtr);

xmlNodePtr XTBChildElementWithTagName(xmlNodePtr, xmlChar *,
									   xmlNodePtr cont=NULL);
std::string XTBValueForNode(xmlNodePtr);
std::string XTBValueWithTagName(xmlNodePtr, xmlChar *);
std::string XTBInnerStringForNode(xmlNodePtr);

std::string XTBReplace(const std::string& str,
					   const std::string& from,
					   const std::string& to);

std::string XTBTrimString(const std::string& str);

std::string XTBAttributeForNode(xmlNodePtr parent, const xmlChar *attrName);

std::string XTBNamespaceForTitle(const std::string&);
std::string XTBPageNameForTitle(const std::string&);
std::string XTBMakeTitle(const std::string& ns,
						 const std::string& pageName);
std::string XTBSanitizeTitle(const std::string&);
std::string XTBLowerStringFor(const std::string&);
std::string XTBUpperStringFor(const std::string&);
std::string XTBDbKeyFor(const std::string&);
std::vector<std::string> XTBSplit(const std::string&,
								  const std::string&);
std::string::size_type XTBFindXMLContentEndPos(const std::string&,
											   std::string::size_type pos=0);

size_t XTBCountOfSubstring(const std::string&, const std::string&);

bool XTBIsTagNameValid(const std::string&);

std::string XTBBytesForSysInt32(uint32_t value);
std::string XTBBytesForSysInt16(uint16_t value);
