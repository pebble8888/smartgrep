#
# makefile for Unix / Linux / MacOS / Windows with MinGW
# Copyright (C) 2011-2013 pebble8888@gmail.com
#
CC=g++
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

