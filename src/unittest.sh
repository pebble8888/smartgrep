#!/bin/bash
# $1: word
# $2: line count
WORD=${1}
OUTFILENAME=./testout/${WORD}.out
LINECOUNT=${2}

./smartgrep -bw ${WORD} > ${OUTFILENAME}

if [ `grep ${WORD} ${OUTFILENAME}|wc -l` -eq ${LINECOUNT} ]; then
   exit 0
else
   exit 1  
fi
