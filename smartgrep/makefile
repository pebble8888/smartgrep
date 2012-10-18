#
# makefile for Unix / Linux / MacOS
# Copyright (C) 2011-2012 pebble8888@gmail.com
#
CC=gcc
TARGET=smartgrep

all:$(TARGET)

OBJS=smartgrep.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $?
	rm -f $(OBJS)
	
.c.o:
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

