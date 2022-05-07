//
//  smartgrep_xctest.m
//  smartgrep_xctest
//
//  Created by pebble8888 on 2015/06/28.
//  Copyright (c) 2015年 pebble8888. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#include "smartgrep.h"

@interface smartgrep_xctest : XCTestCase

@end

@implementation smartgrep_xctest

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

#if 0
- (void)testC {
    char filename[] = "/develop/smartgrep/src/testfile/testsource.c";
    char target_word[] = "c_gamma1";
    int wordtype = 0;
    wordtype |= SG_WORDTYPE_WORD;
    wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
    parse_file(filename, wordtype, target_word);
}
#endif

- (void)testCInclude {
    char filename[] = "/develop/smartgrep/src/testfile/testsource.c";
    char target_word[] = "c_gamma1";
    int wordtype = 0;
    wordtype |= SG_WORDTYPE_NORMAL;
    wordtype |= SG_WORDTYPE_INCLUDE_COMMENT;
    parse_file(filename, wordtype, target_word);
}


#if 0
- (void)testCSharp {
    char filename[] = "/develop/smartgrep/src/testfile/test.cs";
    char target_word[] = "csharp_3";
    int wordtype = 0;
    wordtype |= SG_WORDTYPE_WORD;
    wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
    parse_file(filename, wordtype, target_word);
}
#endif

- (void)testSwift {
    char filename[] = "/Users/pebble8888/develop/smartgrep/src/testfile/test.swift";
    char target_word[] = ".beta?";
    int wordtype = 0;
    wordtype |= SG_WORDTYPE_NORMAL;
    wordtype |= SG_WORDTYPE_EXCLUDE_COMMENT;
    parse_file(filename, wordtype, target_word);
}

@end
