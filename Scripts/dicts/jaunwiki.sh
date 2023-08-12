#!/bin/bash

srcName=jaunwiki

. `dirname "$0"`/../common.sh

export archiveName=ja-wiki
export indexUrl=http://download.uncyclomedia.org/wiki/
export xmlUrl=${indexUrl}${archiveName}.zip
export wikiplexusOptions="-x1 -x2 -x3 -x4 -x5 -x6 -x7 -x8 -x9"
export wikiplexusOptions+=" -x11 -x12 -x13 -x14 -x 15 -x110 -x111 -x117 -m"
export decompressor="funzip"
. `dirname "$0"`/../wikiLoaderWithIndex.sh

exit 0