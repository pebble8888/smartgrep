#!/usr/bin/env bash
# smartgrep installer
# pebble8888@gmail.com

MY_PATH=$HOME/bin

if ! [ -d $MY_PATH ]; then
  mkdir -p $MY_PATH
  echo 
fi

RELATIVE_PATH="`dirname \"$0\"`"
ABSOLUTE_PATH="`( cd \"$RELATIVE_PATH\" && pwd )`"

cd $MY_PATH
ln -sf $ABSOLUTE_PATH/src/smartgrep smartgrep
