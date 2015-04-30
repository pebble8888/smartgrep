#!/bin/bash
# $1: word          can't omit
# $2: line count    can't omit
# $3: option        can omit
WORD=${1}
OUTFILENAME=./testout/${WORD}.out
LINECOUNT=${2}
if [ $# -eq 3 ]; then
  OPTION=${3}
else
  OPTION=""
fi

./smartgrep -i ${OPTION} ${WORD} > ${OUTFILENAME}

if [ `grep ${WORD} ${OUTFILENAME}|wc -l` -eq ${LINECOUNT} ]; then
   exit 0
else
   exit 1  
fi
