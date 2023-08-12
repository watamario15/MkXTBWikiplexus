#!/bin/bash

export workPath=/data/ftppub/xtbpub/derived/intermediate
export outPath=/data/ftppub/xtbpub/derived/xtbdict
export srcPath="${workPath}/${srcName}"
export logPath="${srcPath}/output.log"
export MkXTBWikiplexusPath=/data/Programs/MkXTBWikiplexus
export YomiGenesisPath=/data/Programs/YomiGenesis
export MkRaxPath=/data/Programs/MkRax
export MkXTBIndexDBPath=/data/Programs/MkXTBIndexDB

mkdir -p "$srcPath"

echo "conversion of $srcName starting..." > "$logPath"

echo "MkXTBWikiplexusPath=\"$MkXTBWikiplexusPath\"" >> "$logPath"
echo "YomiGenesisPath=\"$YomiGenesisPath\"" >> "$logPath"
echo "MkRaxPath=\"$MkRaxPath\"" >> "$logPath"
echo "MkXTBIndexDBPath=\"$MkXTBIndexDBPath\"" >> "$logPath"

if [ ! -e "$MkXTBWikiplexusPath" ]; then
    echo "MkXTBWikiplexus not found. aborting." >> "$logPath"
    exit 3
fi

if [ ! -e "$YomiGenesisPath" ]; then
    echo "YomiGenesis not found. aborting." >> "$logPath"
    exit 3
fi

if [ ! -e "$MkRaxPath" ]; then
    echo "MkRax not found. aborting." >> "$logPath"
    exit 3
fi

if [ ! -e "$MkXTBIndexDBPath" ]; then
    echo "MkXTBIndexDB not found. aborting." >> "$logPath"
    exit 3
fi

alias MkXTBWikiplexus="$MkXTBWikiplexusPath"
alias YomiGenesis="$YomiGenesisPath"
alias MkRax="$MkRaxPath"
alias MkXTBIndexDB="$MkXTBIndexDBPath"



