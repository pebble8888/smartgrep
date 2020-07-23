#
# makefile for Unix / Linux / MacOS / Windows with MinGW
# Copyright (C) 2011-2015 pebble8888@gmail.com
#
#CXX=g++
TARGET=smartgrep
LIBS=
#CXXFLAGS=-g
CXXFLAGS=-std=c++14 -Wall

ifeq ($(OS),Windows_NT) 
LIBS+=-lversion -lstdc++
else
LIBS+=-lpthread
endif

all:$(TARGET)

OBJS=smartgrep.o foldernamelist.o prep.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $? $(LIBS)
	rm -f $(OBJS)
	
.c.o:
	$(CXX) $(CXXFLAGS) -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

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
	sh ./unittest.sh vim_one 1
	sh ./unittest.sh vim_four 1
	sh ./unittest.sh vim_five 1
	sh ./unittest.sh vim_six 0
	sh ./unittest.sh hogehtml 1
	sh ./unittest.sh hogecss 1
	sh ./unittest.sh hogescss 1	
	sh ./unittest.sh Crystal 1
	sh ./unittest.sh cifzero 3
	sh ./unittest.sh php_alpha 1
	sh ./unittest.sh fn 1
	sh ./unittest.sh mynameisxml 1

