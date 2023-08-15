#PREFIX=/usr/local/i386-mingw32-4.3.0
#HOST=i386-mingw32
#CC=$(PREFIX)/bin/$(HOST)-gcc
#CXX=$(PREFIX)/bin/$(HOST)-g++
#LD=$(PREFIX)/bin/$(HOST)-g++

CC=clang
CXX=clang++
LD=clang++

CCFLAGS=-Os -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DNDEBUG -I/opt/homebrew/opt/mecab/include -I/opt/homebrew/opt/xz/include
CXXFLAGS=$(CCFLAGS)
LDFLAGS=-L/opt/homebrew/opt/mecab/lib -lmecab -lkakasi -lxml2 -llzma -liconv
TARGETPOSTFIX=-bin
SRCDIR=..

include $(SRCDIR)/Makefile.i
