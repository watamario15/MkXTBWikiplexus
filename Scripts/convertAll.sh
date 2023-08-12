#!/bin/bash
cd `dirname "$0"`/dicts/
for a in *.sh; do
bash "$a"
done


