#
# makefile for Unix / Linux / MacOS
# Copyright (C) 2011-2012 pebble8888@gmail.com
#
CC=gcc
TARGET=smartgrep
LIBS=

ifeq ($(OS),Windows_NT) 
LIBS+=-lversion -lstdc++
endif

all:$(TARGET)

OBJS=smartgrep.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $? $(LIBS)
	rm -f $(OBJS)
	
.c.o:
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

