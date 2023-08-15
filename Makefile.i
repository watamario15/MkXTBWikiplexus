
OBJS_TPLIST=TPLArray.o TPLDictionary.o TPLException.o TPLNumber.o TPLObject.o TPLPropertyListSerialization.o TPLString.o tinyxml.o tinyxmlerror.o tinyxmlparser.o

TARGET_WIKIPLEXUS=MkXTBWikiplexus$(TARGETPOSTFIX)
OBJS_WIKIPLEXUS=CSVIO.o articleProcessor.o articleReader.o main_wikiplexus.o siteInfo.o utils.o XTBArticleGenerator.o XTBDicDB.o XTBRawArticle.o $(OBJS_TPLIST)

TARGET_INDEX=MkXTBIndexDB$(TARGETPOSTFIX)
OBJS_INDEX=main_index.o XTBIndexDB.o CSVIO.o

TARGET_IMAGECOMPLEX=MkImageComplex$(TARGETPOSTFIX)
OBJS_IMAGECOMPLEX=main_imagecomplex.o XTBDicDB.o H2v2.o idct.o jpegdecoder.o utils.o

TARGET_YOMI=YomiGenesis$(TARGETPOSTFIX)
OBJS_YOMI=main_yomi.o toHiragana.o CSVIO.o removeAccent.o

TARGET_MKRAX=MkRax$(TARGETPOSTFIX)
OBJS_MKRAX=mkrax.o

all:	$(TARGET_WIKIPLEXUS) $(TARGET_INDEX) $(TARGET_IMAGECOMPLEX) $(TARGET_YOMI) $(TARGET_MKRAX)

clean:					
	rm -f $(OBJS_TPLIST) $(OBJS_WIKIPLEXUS) $(OBJS_INDEX) $(OBJS_YOMI) $(OBJS_IMAGECOMPLEX) $(OBJS_MKRAX) $(TARGET_WIKIPLEXUS) $(TARGET_INDEX) $(TARGET_IMAGECOMPLEX) $(TARGET_YOMI) $(TARGET_MKRAX)

$(TARGET_WIKIPLEXUS):	$(OBJS_WIKIPLEXUS)
						$(LD) $(OBJS_WIKIPLEXUS) $(LDFLAGS) -o $(TARGET_WIKIPLEXUS)

$(TARGET_INDEX):		$(OBJS_INDEX)
						$(LD) $(OBJS_INDEX) $(LDFLAGS) -o $(TARGET_INDEX)

$(TARGET_IMAGECOMPLEX):	$(OBJS_IMAGECOMPLEX)
						$(LD) $(OBJS_IMAGECOMPLEX) $(LDFLAGS) -o $(TARGET_IMAGECOMPLEX)

$(TARGET_YOMI):			$(OBJS_YOMI)
						$(LD) $(OBJS_YOMI) $(LDFLAGS) -o $(TARGET_YOMI)

$(TARGET_MKRAX):			$(OBJS_MKRAX)
						$(LD) $(OBJS_MKRAX) $(LDFLAGS) -o $(TARGET_MKRAX)



CSVIO.o:				$(SRCDIR)/CSVIO/CSVIO.cpp
						$(CXX) -c $(SRCDIR)/CSVIO/CSVIO.cpp  $(CXXFLAGS)


removeAccent.o:				$(SRCDIR)/YomiGenesis/removeAccent.cpp
						$(CXX) -c $(SRCDIR)/YomiGenesis/removeAccent.cpp  $(CXXFLAGS)

main_imagecomplex.o:	$(SRCDIR)/MkImageComplex/main.cpp
						$(CXX) -c $(SRCDIR)/MkImageComplex/main.cpp  $(CXXFLAGS) -o main_imagecomplex.o

main_index.o:			$(SRCDIR)/MkXTBIndexDB/main.cpp
						$(CXX) -c $(SRCDIR)/MkXTBIndexDB/main.cpp  $(CXXFLAGS) -o main_index.o

XTBIndexDB.o:			$(SRCDIR)/MkXTBIndexDB/XTBIndexDB.cpp
						$(CXX) -c $(SRCDIR)/MkXTBIndexDB/XTBIndexDB.cpp  $(CXXFLAGS)

articleProcessor.o:		$(SRCDIR)/MkXTBWikiplexus/articleProcessor.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/articleProcessor.cpp  $(CXXFLAGS)

articleReader.o:		$(SRCDIR)/MkXTBWikiplexus/articleReader.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/articleReader.cpp  $(CXXFLAGS)

main_wikiplexus.o:		$(SRCDIR)/MkXTBWikiplexus/main.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/main.cpp  $(CXXFLAGS) -o main_wikiplexus.o

siteInfo.o:				$(SRCDIR)/MkXTBWikiplexus/siteInfo.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/siteInfo.cpp  $(CXXFLAGS)

utils.o:				$(SRCDIR)/MkXTBWikiplexus/utils.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/utils.cpp  $(CXXFLAGS) -include $(SRCDIR)/MkXTBWikiplexus/stdafx.h

XTBArticleGenerator.o:	$(SRCDIR)/MkXTBWikiplexus/XTBArticleGenerator.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/XTBArticleGenerator.cpp  $(CXXFLAGS)

XTBDicDB.o:				$(SRCDIR)/MkXTBWikiplexus/XTBDicDB.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/XTBDicDB.cpp  $(CXXFLAGS)

XTBRawArticle.o:		$(SRCDIR)/MkXTBWikiplexus/XTBRawArticle.cpp
						$(CXX) -c $(SRCDIR)/MkXTBWikiplexus/XTBRawArticle.cpp  $(CXXFLAGS)

H2v2.o:				$(SRCDIR)/RichgelJpeg/H2v2.cpp
						$(CXX) -c $(SRCDIR)/RichgelJpeg/H2v2.cpp  $(CXXFLAGS)

idct.o:				$(SRCDIR)/RichgelJpeg/idct.cpp
						$(CXX) -c $(SRCDIR)/RichgelJpeg/idct.cpp  $(CXXFLAGS)

jpegdecoder.o:		$(SRCDIR)/RichgelJpeg/jpegdecoder.cpp
						$(CXX) -c $(SRCDIR)/RichgelJpeg/jpegdecoder.cpp  $(CXXFLAGS)

main_yomi.o:			$(SRCDIR)/YomiGenesis/main.cpp
						$(CXX) -c $(SRCDIR)/YomiGenesis/main.cpp  $(CXXFLAGS) -o main_yomi.o

toHiragana.o:			$(SRCDIR)/YomiGenesis/toHiragana.cpp
						$(CXX) -c $(SRCDIR)/YomiGenesis/toHiragana.cpp  $(CXXFLAGS)

TPLArray.o:				$(SRCDIR)/TPList/TPLArray.cpp
						$(CXX) -c $< $(CXXFLAGS)

mkrax.o:				$(SRCDIR)/MkRax/mkrax.cpp
						$(CXX) -c $< $(CXXFLAGS)

TPLDictionary.o:		$(SRCDIR)/TPList/TPLDictionary.cpp
	$(CXX) -c $< $(CXXFLAGS)

TPLException.o:			$(SRCDIR)/TPList/TPLException.cpp
	$(CXX) -c $< $(CXXFLAGS)

TPLNumber.o:			$(SRCDIR)/TPList/TPLNumber.cpp
	$(CXX) -c $< $(CXXFLAGS)

TPLObject.o:			$(SRCDIR)/TPList/TPLObject.cpp
	$(CXX) -c $< $(CXXFLAGS)

TPLPropertyListSerialization.o:		$(SRCDIR)/TPList/TPLPropertyListSerialization.cpp
	$(CXX) -c $< $(CXXFLAGS)

TPLString.o:				$(SRCDIR)/TPList/TPLString.cpp
	$(CXX) -c $< $(CXXFLAGS)

tinyxml.o:					$(SRCDIR)/TinyXML/tinyxml.cpp
	$(CXX) -c $< $(CXXFLAGS)

tinyxmlerror.o:					$(SRCDIR)/TinyXML/tinyxmlerror.cpp
	$(CXX) -c $< $(CXXFLAGS)

tinyxmlparser.o:					$(SRCDIR)/TinyXML/tinyxmlparser.cpp
	$(CXX) -c $< $(CXXFLAGS)










TWRenderDCTcw.o:	../TWRenderDCTcw.cpp XTBook_Prefix.pch.gch
	$(CXX) -include XTBook_Prefix.pch -c ../TWRenderDCTcw.cpp -o TWRenderDCTcw.o $(CFLAGS) $(CXXFLAGS) -I..
