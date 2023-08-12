//
//  main.cpp
//  YomiGenesis
//
//  Created by Kawada Tomoaki on 7/27/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//


#include "stdafx.h"
#include <mecab.h>
#include <iconv.h>
#include "../CSVIO/CSVIO.h"
#include <set>
#include <algorithm>
extern "C"{
#include <libkakasi.h>
};
#include <string.h>
CSVIOReader *g_csvReader;
CSVIOWriter *g_csvWriter;

mecab_t *g_mecab;

std::set<std::string> g_allowedChars;

std::string removeAccent(std::string s);

static std::string iconvString(const std::string& str, iconv_t ic){
	const size_t outputBufSize=1024;
	const char *inputPtr=str.c_str();
	char outputBuf[outputBufSize];
	char *outputPtr;
	std::string outputStr;
	size_t inputLen=str.size();
	size_t outputLen;
	
	while(inputLen!=0){
		outputPtr=outputBuf;
		outputLen=outputBufSize;
		size_t oldInputLen=inputLen;
		iconv(ic, const_cast<char **>(&inputPtr), &inputLen, &outputPtr, &outputLen);
		if(inputLen==oldInputLen){
			throw std::string("error in iconv");
		}
		outputStr.append(outputBuf, outputBufSize-outputLen);
	}
	return outputStr;
}

static std::string iconvString(const std::string& str, const char *toCode,
						const char *fromCode){
	struct Context{
		iconv_t ic;
		Context(const char *toCode,
				const char *fromCode){
			ic=iconv_open(toCode, fromCode);
		}
		~Context(){
			iconv_close(ic);
		}
	} context(toCode, fromCode);
	return iconvString(str, context.ic);
}

/* "trim" string. */
static std::string trimString(std::string str){
	size_t pos, ln;
	const char *ngs="\n\r \t";
	pos=0; ln=str.size();
	if(!str.size())
		return str;
	
	// beginning
	while(ln){
		if(strchr(ngs, str[pos])){
			pos++; ln--;	
		}else{
			break;
		}
	}
	
	// ending
	while(ln){
		if(strchr(ngs, str[pos+ln-1])){
			ln--;
		}else{
			break;
		}
	}
	
	// return
	return str.substr(pos, ln);
}

static std::string replaceString(const std::string& str, const std::string& from, const std::string& to){
	std::string::size_type pos=0;
	std::string newStr;
	while(pos<str.size()){
		std::string::size_type nextPos;
		nextPos=str.find(from, pos);
		if(nextPos==std::string::npos)
			break;
		newStr+=str.substr(pos, nextPos-pos);
		newStr+=to;
		pos=nextPos+from.size();
	}
	newStr+=str.substr(pos);
	return newStr;
}

static std::vector<std::string> splitString(const std::string& str, const std::string& delimiter){
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

static std::string tryKakasi(const std::string& str){
	char *ret=kakasi_do(const_cast<char *>(str.c_str()));;
	std::string s=ret;
	//kakasi_free(ret);
	return s;
}

static std::string yomiForText(const std::string& str){
	std::string str2=replaceString(str, "〜", "ー");;
	str2=iconvString(str2, "euc-jp//TRANSLIT//IGNORE", "utf-8");
	
	mecab_t *mecab=g_mecab;

	const mecab_node_t *node;
	std::string outString;
	
	node=mecab_sparse_tonode2(mecab, str2.data(), str2.size());
	
	if(node==NULL){
		throw std::string("error in mecab");
	}
	
	node=node->next;
	
	for(;node->next!=NULL;node=node->next){
		std::vector<std::string> features;
		features=splitString(node->feature, ",");
		if(features.size()<=7 || features[7]=="*"){
			// no yomi
			// try kakasi.
			std::string s=tryKakasi(std::string(node->surface, node->length));
			if(s.empty())
			outString.append(node->surface, node->length);
			else
				outString+=s;
		}else{
			outString+=features[7];
		}
	}
	
	
	return iconvString(outString, "utf-8", "euc-jp");
}

static std::string removeSpaces(const std::string& str){
	std::string outStr;
	outStr.reserve(str.size());
	
	for(size_t i=0;i<str.size();i++){
		if(str[i]==' ' || str[i]=='\n' || str[i]=='\r')
			continue;
		outStr+=str[i];
	}
	return outStr;
}

std::string toHiragana(std::string s);

static char easytoLower(char c){
	if(c>='A' && c<='Z')
		return c-('A'-'a');
	return c;
}

static void toLowerInplace(std::string& s){
	std::transform(s.begin(), s.end(), s.begin(), easytoLower);
}

static std::string removeSymbols(const std::string& s){
	size_t i=0;
	std::string outStr;
	
	while(i<s.size()){
		std::string sub;
		if((s[i]&0x80)==0){
			sub=s.substr(i, 1);
			i+=1;
		}else if((s[i]&0x40)==0){
			// invalid
			i++; continue;
		}else if((s[i]&0x20)==0){
			sub=s.substr(i, 2);
			i+=2;
		}else if((s[i]&0x10)==0){
			sub=s.substr(i, 3);
			i+=3;
		}else if((s[i]&0x08)==0){
			sub=s.substr(i, 4);
			i+=2;
		}else if((s[i]&0x04)==0){
			sub=s.substr(i, 5);
			i+=2;
		}else if((s[i]&0x02)==0){
			sub=s.substr(i, 6);
			i+=2;
		}else{
			// invalid
			i++; continue;
		}
		
		if(g_allowedChars.find(sub)==g_allowedChars.end())
			continue;
		
		outStr+=sub;
	}
	
	return outStr;
}

static void initAllowedChars(){
	const char *chars[]={
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n",
		"o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
		"あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ",
		"さ", "し", "す", "せ", "そ", "た", "ち", "つ", "て", "と",
		"な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ",
		"ま", "み", "む", "め", "も", "や", "ゆ", "よ", 
		"ら", "り", "る", "れ", "ろ", "わ", "ゐ", "ゑ", "を", "ん",
		"が", "ぎ", "ぐ", "げ", "ご", "ざ", "じ", "ず", "ぜ", "ぞ",
		"だ", "ぢ", "づ", "で", "ど", "ば", "び", "ぶ", "べ", "ぼ",
		"ぱ", "ぴ", "ぷ", "ぺ", "ぽ", "ぁ", "ぃ", "ぅ", "ぇ", "ぉ",
		"ゕ", "ゖ", "っ", "ゃ", "ゅ", "ょ", "ゎ", "ゔ",
		"か゚", "き゚", "く゚", "け゚", "こ゚", "ゝ", "ゞ", "ゟ",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"ー",
		NULL
	};
	for(const char **ptr=chars;*ptr;ptr++){
		g_allowedChars.insert(*ptr);
	}
}

std::string avoidDangeroudChars(std::string s);

int main (int argc, const char * argv[]){
	initAllowedChars();
	
	g_csvReader=new CSVIOReader(stdin, false);
	g_csvWriter=new CSVIOWriter(stdout, false);
	
	g_mecab=mecab_new2("-d /var/lib/mecab/dic/ipadic");
	if(!g_mecab){
		fprintf(stderr, "fatal error: error in mecab.\n");
		return 1;
	}
	
	const char *kakasiOptions[]={
		"kakasi",
		"-ieuc",
		"-oeuc",
		"-JH",
		NULL
	};
	if(kakasi_getopt_argv(4, const_cast<char **>((const char **)kakasiOptions))){
		fprintf(stderr, "fatal error: error in kakasi.\n");
		return 1;
	}
	
	CSVIORecord record;
	while(record=g_csvReader->readRecord(), !record.empty()){
		try{
			std::string s=record[0];
			std::string s2;
			// avoid error in iconv
			s=avoidDangeroudChars(s);
			s=removeAccent(s);
			s=yomiForText(s);
			if(s.empty()){
				fprintf(stderr, "warning: no yomi for \"%s\" (error?)\n",record[0].c_str());
				continue;
			}
			s=toHiragana(s);
			s=removeSpaces(s);
			toLowerInplace(s);
			if(s.empty()){
				fprintf(stderr, "warning: no output emitted for \"%s\" (with symbols)\n",record[0].c_str());
				continue;
			}
			s2=s;
			s=removeSymbols(s);
			if(s.empty()){
				fprintf(stderr, "warning: no output emitted for \"%s\" (without symbols, "
						"with symbols: \"%s\")\n",record[0].c_str(), s2.c_str());
			
				continue;
			}
			record[0]=s;
			g_csvWriter->writeRecord(record);
			
			if(s.find("ー")!=std::string::npos){
				s=replaceString(s, "ー", "");
				record[0]=s;
				g_csvWriter->writeRecord(record);
			}
		}catch(const std::string& s){
			fprintf(stderr, "error while processing \"%s\": %s\n", record[0].c_str(), s.c_str());
		}
		
	}
	
	delete g_csvReader;
	delete g_csvWriter;
}

