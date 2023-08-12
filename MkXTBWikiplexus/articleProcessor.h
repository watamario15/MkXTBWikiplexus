//
//  articleProcessor.h
//  MkXTBWikiplexus
//
//  Created by Kawda Tomoaki on 7/3/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"

class XTBDicDB;

void XTBProcessArticle(const std::string& title,
					   const std::string& text,
					   XTBDicDB *articleDb,
					   XTBDicDB *templateDb,
					   FILE *titlesList,
					   FILE *baseNamesList);

