#!/bin/bash


export rssPath="${srcPath}/rss.xml"
export newRssPath="${srcPath}/rss-new.xml"
export infoPath="${srcPath}/Info.plist"
export pidPath="${srcPath}/Last.pid"

echo "xmlUrl = \"$xmlUrl\"" >> "$logPath"
echo "rssUrl = \"$rssUrl\"" >> "$logPath"
echo "srcPath = \"$srcPath\"" >> "$logPath"
echo "rssPath = \"$rssPath\"" >> "$logPath"
echo "newRssPath = \"$newRssPath\"" >> "$logPath"
echo "infoPath = \"$infoPath\"" >> "$logPath"
echo "wikiplexusOptions = \"$wikiplexusOptions\"" >> "$logPath"

# create working directory.
mkdir -p "$srcPath"

# if already running, bail out.
if [ -e "$newRssPath" ]; then

	if [ -e "$pidPath" ]; then
		# check by pid.
		runningPid=`cat "$pidPath"`
		if kill -0 "$runningPid"; then
			# running.
			echo "this is running now ($runningPid). aborting." >> "$logPath"
			exit 3
		fi 

	else
		echo "new rss already exists. may be this is running now. aborting." >> "$logPath"
		exit 3
	fi

fi

if [ -e "$pidPath" ]; then
	# check by pid.
	runningPid=`cat "$pidPath"`
	if kill -0 "$runningPid"; then
	# running.
	echo "this is running now ($runningPid). aborting." >> "$logPath"
	exit 3
	fi
fi

echo "pid = $$" >> "$logPath"
echo $$ > "$pidPath"

# make sure Info.plist exists.
if [ ! -e "$infoPath" ]; then
echo "Info.plist doesn't exist. aborting." >> "$logPath"
exit 3
fi

# make sure old rss exists because
# diff might fail without it.
touch "$rssPath"

# download RSS.
echo "curl \"$rssUrl\"" >> "$logPath"
curl --retry 16 "$rssUrl" > "$newRssPath" 2>> "$logPath" || { echo "curl failed!" >> "$logPath"; exit 1; }

# compare RSS and check update.
if diff -q "$rssPath" "$newRssPath" >/dev/null ; then
updated=no
else
updated=yes
fi

# if no update, bail out.
if [ "$updated" == "no" ]; then
echo "not updated." >> "$logPath"
rm -f "$newRssPath"
exit 0
fi

# parse RSS and check date.
# first, find <link> element.
linkLine="`cat $newRssPath|grep \<link\>|head -1`"

# this looks like:
# <link>http://download.wikimedia.org/jawiki/20110921</link>
# now, separate this by slash.
IFS=/
linkLine="${linkLine/\/\//\/}"
linkLine="${linkLine/\/\//\/}"
set -- ${linkLine/\<\/link\>/}

# get last part.
while [ "$2" != "" ]; do
shift
done

srcDate="$1"

# assert( strlen(srcDate) == 8 )
if [ "${#srcDate}" != "8" ]; then
echo "assertion failed: strlen(srcDate) == 8." >> "$logPath"
echo "srcDate = \"$srcDate\"" >> "$logPath"
echo "strlen(srcDate) = \"${#srcDate}\"" >> "$logPath"
echo "linkLine = \"$linkLine\"" >> "$logPath"
rm -f "$newRssPath"
exit 2
fi

# it is the path for the output bundle.
outBundleName="${srcName}-${srcDate}.xtbdict"
outBundle="$outPath/${outBundleName}"
echo "outBundle = \"$outBundle\"" >> "$logPath"

# output bundle shouldn't exist yet.
if [ -e "$outBundle" ] ; then
echo "strangely, it already exists. aborting." >> "$logPath"
rm -f "$newRssPath"
exit 2
fi

# create bundle.
mkdir -p "$outBundle"

# mark RSS is read.
mv "$newRssPath" "$rssPath"

# conversion starting with downloading, decompression, and recompression.
curl --retry 16 "$xmlUrl" | bunzip2 | \
time "$MkXTBWikiplexusPath" -o "$outBundle" -s "$wikiplexusOptions" 2>> "$logPath" | \
"$MkRaxPath" -o "$outBundle"/Articles.db.rax 2>> "$logPath" >> "$logPath"

# Generate Yomi.
"$YomiGenesisPath" < "$outBundle"/BaseNames.csv > "$outBundle"/Yomis.csv \
2>> "$logPath"

# Create IndexDB.
if [ "$yomiOnly" == "yes" ]; then
"$MkXTBIndexDBPath" -o "$outBundle"/Search "$outBundle"/Yomis.csv
else
"$MkXTBIndexDBPath" -o "$outBundle"/Search "$outBundle"/*.csv
fi

# Remove CSVs.
rm -f "$outBundle"/*.csv

# Copy Info.plist
cp "$infoPath" "$outBundle"/Info.plist 2>> "$logPath"

# remove old archived.
# it is okay to remove now. old archive must be on Google Docs.
cd "$outBundle"/../
rm -f "$srcName-*".tar.xz

# compress the output bundle.
tar -c "$outBundleName" | xz > "$outBundleName".tar.xz 2>> "$logPath"

# dump files list.
ls -l "$outBundleName"/ >> "$logPath"

# remove uncompressed data.
rm -Rf "$outBundleName"

# remove pid.
rm -f "$pidPath"
