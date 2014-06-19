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


OBJS=smartgrep.o foldernamelist.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $? $(LIBS)
	rm -f $(OBJS)
	
.c.o:
	$(CC) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f ./testout/*.out

test:
	sh ./unittest.sh c_gamma1 1
	sh ./unittest.sh js_dog 1
	sh ./unittest.sh js_dog 0 --nojs
	sh ./unittest.sh java_good 1
	sh ./unittest.sh scala_good 1
	sh ./unittest.sh go_hello 1 
	sh ./unittest.sh py_alpha1 0 
	sh ./unittest.sh py_gamma1 0 
	sh ./unittest.sh py_alpha2 0 
	sh ./unittest.sh py_alpha3 1 
	sh ./unittest.sh py_B 1 
	sh ./unittest.sh py_C 0 
	sh ./unittest.sh py_D 0
	sh ./unittest.sh py_E 0
	sh ./unittest.sh rb_alpha1 1
	sh ./unittest.sh rb_gamma2 0
	sh ./unittest.sh rb_hoge1 1
	sh ./unittest.sh rb_gamma1 1
	sh ./unittest.sh rb_delta2 0
	sh ./unittest.sh rb_delta3 0
	sh ./unittest.sh rb_delta4 1
	sh ./unittest.sh coffee_hello 1
	sh ./unittest.sh perl_1 1
	sh ./unittest.sh perl_2 0
	sh ./unittest.sh perl_4 1
	sh ./unittest.sh perl_7 0
	sh ./unittest.sh csharp_1 0
	sh ./unittest.sh csharp_2 0
	sh ./unittest.sh csharp_3 1
	sh ./unittest.sh vbdotnet_1 4
	sh ./unittest.sh vbdotnet_2 0
	sh ./unittest.sh vbdotnet_3 0
	sh ./unittest.sh vbdotnet_4 0
	sh ./unittest.sh vbdotnet_6 4
	sh ./unittest.sh shell_alpha1 0 
	sh ./unittest.sh shell_beta1 0 
	sh ./unittest.sh shell_gamma1 1 
	sh ./unittest.sh erb_email 2
	sh ./unittest.sh stringstest 1
	sh ./unittest.sh testpbxproj 1
	sh ./unittest.sh testplist 1
	sh ./unittest.sh swift_alpha 2
	sh ./unittest.sh storyboardtest 1

