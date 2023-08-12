#!/bin/bash

srcName=frwiki

. `dirname "$0"`/../common.sh

export xmlUrl=http://dumps.wikimedia.org/${srcName}/latest/${srcName}-latest-pages-articles.xml.bz2
export rssUrl=${xmlUrl}-rss.xml
export wikiplexusOptions="-x1 -x2 -x3 -x4 -x5 -x6 -x7 -x8 -x9"
export wikiplexusOptions+=" -x11 -x12 -x13 -x14 -x 15 -x110 -x111 -x117 -m"
export yomiOnly=yes

. `dirname "$0"`/../wikiLoaderWithRss.sh

exit 0
