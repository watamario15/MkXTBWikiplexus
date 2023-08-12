#!/bin/bash


export indexPath="${srcPath}/index.htm"
export newIndexPath="${srcPath}/index-new.htm"
export infoPath="${srcPath}/Info.plist"
export pidPath="${srcPath}/Last.pid"
if [ "$decompressor" == "" ]; then
export decompressor="bunzip2"
fi

echo "xmlUrl = \"$xmlUrl\"" >> "$logPath"
echo "indexUrl = \"$indexUrl\"" >> "$logPath"
echo "srcPath = \"$srcPath\"" >> "$logPath"
echo "indexPath = \"$indexPath\"" >> "$logPath"
echo "newIndexPath = \"$newIndexPath\"" >> "$logPath"
echo "infoPath = \"$infoPath\"" >> "$logPath"
echo "archiveName = \"$archiveName\"" >> "$logPath"
echo "wikiplexusOptions = \"$wikiplexusOptions\"" >> "$logPath"
echo "decompressor = \"$decompressor\"" >> "$logPath"
echo "archiveName = \"$archiveName\"" >> "$logPath"


# create working directory.
mkdir -p "$srcPath"

# if already running, bail out.
if [ -e "$newIndexPath" ]; then

	if [ -e "$pidPath" ]; then
		# check by pid.
		runningPid=`cat "$pidPath"`
		if kill -0 "$runningPid"; then
			# running.
			echo "this is running now ($runningPid). aborting." >> "$logPath"
			exit 3
		fi 

	else
		echo "new index already exists. may be this is running now. aborting." >> "$logPath"
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
touch "$indexPath"

# download RSS.
echo "curl \"$indexUrl\"" >> "$logPath"
curl --retry 16 "$indexUrl" > "$newIndexPath" 2>> "$logPath" || { echo "curl failed!" >> "$logPath"; exit 1; }
indexData="`grep $archiveName < $newIndexPath | head -1`"
echo "$indexData" > "$newIndexPath"

# not published?
if [ "$indexData" == "" ]; then
	echo "not published." >> "$logPath"
	rm -f "$newIndexPath"
	exit 0
fi

# compare RSS and check update.
if diff -q "$indexPath" "$newIndexPath" >/dev/null ; then
updated=no
else
updated=yes
fi

# if no update, bail out.
if [ "$updated" == "no" ]; then
	echo "not updated." >> "$logPath"
	rm -f "$newIndexPath"
	rm -f "$pidPath"
	exit 0
fi

# parse index and check date.
# first, find <link> element.
linkLine="`cat $newIndexPath`"

# this looks like:
# <tr><td valign="top"><img src="/icons/compressed.gif" alt="[ ]">
# </td><td><a href="ja-wiki.zip">ja-wiki.zip</a></td>
# <td align="right">22-Aug-2011 14:44 </td>
# <td align="right"> 77M</td></tr>
# now, separate this by hyphen.
IFS=-
linkLine="${linkLine/--/-}"
linkLine="${linkLine/--/-}"
set -- ${linkLine//$archiveName/}

dayPart="$1"
monthPart="$2"
yearPart="$3"

# get last part for day.
IFS=\>
set -- ${dayPart//\>\>/}
while [ "$2" != "" ]; do
shift
done
dayPart="$1"

# get first part for year.
IFS=" "
set -- ${yearPart}
yearPart="$1"

# if day is 1-char long, add '0' prefix.
if [ "${dayPart:1:1}" == "" ]; then
	dayPart=0$dayPart
fi


# extract month.
if [ "$monthPart" == "Jan" ]; then
monthPart=01
elif [ "$monthPart" == "Feb" ]; then
monthPart=02
elif [ "$monthPart" == "Mar" ]; then
monthPart=03
elif [ "$monthPart" == "Apr" ]; then
monthPart=04
elif [ "$monthPart" == "May" ]; then
monthPart=05
elif [ "$monthPart" == "Jun" ]; then
monthPart=06
elif [ "$monthPart" == "Jul" ]; then
monthPart=07
elif [ "$monthPart" == "Aug" ]; then
monthPart=08
elif [ "$monthPart" == "Sep" ]; then
monthPart=09
elif [ "$monthPart" == "Oct" ]; then
monthPart=10
elif [ "$monthPart" == "Nov" ]; then
monthPart=11
elif [ "$monthPart" == "Dec" ]; then
monthPart=12
fi


srcDate=${yearPart}${monthPart}${dayPart}

# assert( strlen(srcDate) == 8 )
if [ "${#srcDate}" != "8" ]; then
echo "assertion failed: strlen(srcDate) == 8." >> "$logPath"
echo "srcDate = \"$srcDate\"" >> "$logPath"
echo "strlen(srcDate) = \"${#srcDate}\"" >> "$logPath"
echo "linkLine = \"$linkLine\"" >> "$logPath"
rm -f "$newIndexPath"
rm -f "$pidPath"
exit 2
fi

# it is the path for the output bundle.
outBundleName="${srcName}-${srcDate}.xtbdict"
outBundle="$outPath/${outBundleName}"
echo "outBundle = \"$outBundle\"" >> "$logPath"

# output bundle shouldn't exist yet.
if [ -e "$outBundle" ] ; then
echo "strangely, it already exists. aborting." >> "$logPath"
rm -f "$newIndexPath"
rm -f "$pidPath"
exit 2
fi

# create bundle.
mkdir -p "$outBundle"

# mark RSS is read.
mv "$newIndexPath" "$indexPath"

# conversion starting with downloading, decompression, and recompression.
curl --retry 16 "$xmlUrl" | ${decompressor} | \
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
