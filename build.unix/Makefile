
#PREFIX=/usr/local/i386-mingw32-4.3.0
#HOST=i386-mingw32
#CC=$(PREFIX)/bin/$(HOST)-gcc
#CXX=$(PREFIX)/bin/$(HOST)-g++
#LD=$(PREFIX)/bin/$(HOST)-g++

CC=gcc
CXX=g++
LD=g++

CCFLAGS=-Os -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DNDEBUG -I/usr/include/libxml2
CXXFLAGS=$(CCFLAGS)
LDFLAGS=-liconv -L/usr/local/lib -lxml2 -lmecab -lkakasi -llzma
TARGETPOSTFIX=-bin
SRCDIR=..

include $(SRCDIR)/Makefile.i



