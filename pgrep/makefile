#
# makefile for Unix / Linux / MacOS
# Copyright (C) 2011 pebble
#
CC=gcc
TARGET=pgrep

all:$(TARGET)

OBJS=pgrep.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $?
	rm -f $(OBJS)
	
.c.o:
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

