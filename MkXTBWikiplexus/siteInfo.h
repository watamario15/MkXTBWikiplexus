//
//  siteInfo.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"

enum{
	TWStandardNamespaceInvalid=-100,
	TWStandardNamespaceMedia=-2,
	TWStandardNamespaceSpecial=-1,
	TWStandardNamespaceMain=0,
	TWStandardNamespaceTalk,
	TWStandardNamespaceUser,
	TWStandardNamespaceUserTalk,
	TWStandardNamespaceProject,
	TWStandardNamespaceProjectTalk,
	TWStandardNamespaceFile,
	TWStandardNamespaceFileTalk,
	TWStandardNamespaceMediaWiki,
	TWStandardNamespaceMediaWikiTalk,
	TWStandardNamespaceTemplate,
	TWStandardNamespaceTemplateTalk,
	TWStandardNamespaceHelp,
	TWStandardNamespaceHelpTalk,
	TWStandardNamespaceCategory,
	TWStandardNamespaceCategoryTalk
};

void XTBExpandSiteInfo(xmlNodePtr);

void XTBWriteSiteInfo(const std::string& path);

std::string XTBNameForStandardNamespace(int);
bool XTBIsNamespaceExcluded(const std::string&);
void XTBExcludeNamespace(const std::string&);

bool XTBDoesNamespaceExists(const std::string&);