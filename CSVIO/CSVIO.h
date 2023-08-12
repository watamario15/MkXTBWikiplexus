//
//  CSVIO.h
//  MkXTBWikiplexus
//
//  Created by Kawada Tomoaki on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string>
#include <vector>

// empty record is EOF for reader
typedef std::vector<std::string> CSVIORecord;

class CSVIOReader{
	FILE *m_fileHandle;
	char *m_buffer;
	bool m_shouldClose;
public:
	CSVIOReader(const char *);
	CSVIOReader(FILE *, bool);
	~CSVIOReader();
	
	CSVIORecord readRecord();
};

class CSVIOWriter{
	FILE *m_fileHandle;
	bool m_shouldClose;
	std::string escape(const std::string&);
public:
	CSVIOWriter(const char *);
	CSVIOWriter(FILE *, bool);
	~CSVIOWriter();
	
	void writeRecord(const CSVIORecord&);
};
