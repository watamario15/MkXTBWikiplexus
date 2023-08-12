//
//  articleReader.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/2/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include "stdafx.h"

typedef void (*XTBArticleReaderCallback)(xmlNodePtr, void *, long);
typedef void (*XTBArticleReaderSiteinfoCallback)(xmlNodePtr, void *, long);

void XTBReadArticles(XTBArticleReaderCallback, 
					 XTBArticleReaderSiteinfoCallback,
					 int fromFd, void *param);
