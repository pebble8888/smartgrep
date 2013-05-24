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

test:
	sh ./unittest.sh c_gamma1 1
	sh ./unittest.sh java_good 1
	sh ./unittest.sh scala_good 1
	sh ./unittest.sh go_hello 1 
	sh ./unittest.sh py_alpha1 0 
	sh ./unittest.sh py_gamma1 0 
	sh ./unittest.sh py_alpha2 0 
	sh ./unittest.sh py_alpha3 1 
	sh ./unittest.sh py_B 1 
	sh ./unittest.sh py_C 0 
	sh ./unittest.sh rb_alpha1 1
	sh ./unittest.sh rb_gamma2 0
	sh ./unittest.sh rb_hoge1 1
	sh ./unittest.sh rb_gamma1 1
	sh ./unittest.sh rb_delta2 0
	sh ./unittest.sh coffee_hello 1
