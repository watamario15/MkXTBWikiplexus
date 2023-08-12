//
//  siteInfo.cpp
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "siteInfo.h"
#include "../TPList/TPLAutoReleasePtr.h"
#include "../TPList/TPLPropertyListSerialization.h"
#include "../TPList/TPLDictionary.h"
#include "utils.h"
#include "../TPList/TPLString.h"
#include "../TPList/TPLArray.h"
#include "../TPList/TPLNumber.h"

static std::string g_siteName="Wikipedia";
static std::string g_baseUrl="http://www.nexhawks.net";
static std::string g_generator="multimediaTCPPSH";
static std::map<int, std::string> g_namespaces;
static std::set<std::string> g_excludedNamespaces;
static std::set<std::string> g_existingNamespaces;

static void expandNamespaces(xmlNodePtr node){
	xmlNodePtr child=node->children;
	while(child){
		if(!xmlStrcasecmp(child->name, BAD_CAST "namespace")){
			std::string nsKey;
			std::string nsName;
			nsKey=XTBAttributeForNode(child, BAD_CAST "key");
			nsName=XTBValueForNode(child);
			
			int key=atoi(nsKey.c_str());
			g_namespaces[key]=nsName;
			g_existingNamespaces.insert(nsName);
		}
		
		child=child->next;
	}
}

void XTBExpandSiteInfo(xmlNodePtr node){
	xmlNodePtr child=node->children;
	
	while(child){
		
		if(!xmlStrcasecmp(child->name, BAD_CAST "sitename")){
			g_siteName=XTBValueForNode(child);
		}else if(!xmlStrcasecmp(child->name, BAD_CAST "base")){
			g_baseUrl=XTBValueForNode(child);
		}else if(!xmlStrcasecmp(child->name, BAD_CAST "generator")){
			g_generator=XTBValueForNode(child);
		}else if(!xmlStrcasecmp(child->name, BAD_CAST "namespaces")){
			expandNamespaces(child);
		}
		
		child=child->next;
	}
}

static TPLDictionary *dictionaryForNamespace(int key, const std::string& name){
	TPLAutoReleasePtr<TPLDictionary> dic(new TPLDictionary());
	{
		TPLAutoReleasePtr<TPLNumber> str=
		new TPLNumber(key);
		dic->setObject(&(*str), "XTBWikiplexusNamespaceKey");
	}
	{
		TPLAutoReleasePtr<TPLString> str=
		new TPLString(name.c_str());
		dic->setObject(&(*str), "XTBWikiplexusNamespaceName");
	}
	return static_cast<TPLDictionary *>(dic->retain());
}

static TPLArray *arrayForNamespaces(){
	TPLAutoReleasePtr<TPLArray> ary(new TPLArray());
	
	for(std::map<int, std::string>::iterator it=
		g_namespaces.begin();it!=g_namespaces.end();it++){
		
		{
			TPLAutoReleasePtr<TPLDictionary> d=
			dictionaryForNamespace(it->first, it->second);
			ary->addObject(&(*d));
		}
		
	}
	
	return static_cast<TPLArray *>(ary->retain());
}

void XTBWriteSiteInfo(const std::string& path){
	TPLAutoReleasePtr<TPLDictionary> dic(new TPLDictionary());
	
	{
		TPLAutoReleasePtr<TPLString> str=
		new TPLString(g_siteName.c_str());
		dic->setObject(&(*str), "XTBWikiplexusSiteName");
	}
	
	{
		TPLAutoReleasePtr<TPLString> str=
		new TPLString(g_baseUrl.c_str());
		dic->setObject(&(*str), "XTBWikiplexusBaseUrl");
	}
	
	{
		TPLAutoReleasePtr<TPLString> str=
		new TPLString(g_generator.c_str());
		dic->setObject(&(*str), "XTBWikiplexusGenerator");
	}
	
	{
		TPLAutoReleasePtr<TPLArray> d=
		arrayForNamespaces();
		dic->setObject(&(*d), "XTBWikiplexusNamespaces");
	}
	
	FILE *f=fopen(path.c_str(), "wb");
	if(!f){
		fprintf(stderr, "error: cannot open %s for writing",
				path.c_str());
		exit(2);
	}
	
	TPLPropertyListSerialization::writePropertyList(&(*dic), f);
	
	fclose(f);
}


std::string XTBNameForStandardNamespace(int i){
	if(g_namespaces.find(i)==g_namespaces.end())
		return "!NONS";
	return g_namespaces[i];
}

bool XTBIsNamespaceExcluded(const std::string& name){
	if(g_excludedNamespaces.find(name)!=g_excludedNamespaces.end())
		return true;
	else
		return false;
}

void XTBExcludeNamespace(const std::string& name){
	g_excludedNamespaces.insert(name);
}

bool XTBDoesNamespaceExists(const std::string& name){
	if(g_existingNamespaces.find(name)!=g_existingNamespaces.end())
		return true;
	else
		return false;
}
