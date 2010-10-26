/* Copyright (c) 2010 GLsoft.mobi
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 */

#import "LGGOGraph.h"
#import "LGGOGraphContext.h"

#import "LGGONumber.h"

#import "LGGONumberTests.h"

#define LGGO_MAX_INT59 ((int64_t)0x03ffffffffffffff)
#define LGGO_MIN_INT59 ((int64_t)(-1*LGGO_MAX_INT59))

@interface LGGONumberTests () {
  NSSet *signedIntegers;
  NSSet *floats;
  NSSet *boundaries;

  void (^checkNumberEquivalence)(id obj, BOOL *stop);
}
@end

@implementation LGGONumberTests

- (void) setUp {
  [super setUp];
  
  /*
   This block just creates a tagged number from a number, and tests for
   equality. That is actually a fairly good test, since it exercises a lot of
   NSNumbers's machinery. This test found a lot of byte encoding issues, and
   issues with 64 bit encoding compression.
   */
  checkNumberEquivalence = [^(id number, BOOL* stop){
    LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
    LGGOGraphContext *context = [graph newContext];
    
    LGGONumber *testNumber = [[LGGONumber alloc] initWithNumber:number inContext:context];
    STAssertTrue([testNumber isEqual:number], nil);
    [testNumber release];
    
    [context release];
    [graph release];
  } copy];
  
  //FIXME these boundaries are wrong, our new encodings have better fidelity
  boundaries = [[NSSet alloc] initWithObjects:
  //              [NSNumber numberWithUnsignedLongLong:LGGO_MAX_INT59],
                [NSNumber numberWithLongLong:LGGO_MIN_INT59],
	//							[NSNumber numberWithUnsignedInteger:65536],
                nil];
                
  signedIntegers = [[NSSet alloc] initWithObjects:
                      [NSNumber numberWithInt:0],
                      [NSNumber numberWithInt:1],
                      [NSNumber numberWithInt:2],
                      [NSNumber numberWithInt:3],
                      [NSNumber numberWithInt:4],
                      [NSNumber numberWithInt:5],
                      [NSNumber numberWithInt:6],
                      [NSNumber numberWithInt:7],
                      [NSNumber numberWithInt:8],
                      [NSNumber numberWithInt:9],
                      [NSNumber numberWithInt:10],
                      [NSNumber numberWithInt:-1],
                      [NSNumber numberWithInt:-2],
                      [NSNumber numberWithInt:-3],
                      [NSNumber numberWithInt:-4],
                      [NSNumber numberWithInt:-5],
                      [NSNumber numberWithInt:-6],
                      [NSNumber numberWithInt:-7],
                      [NSNumber numberWithInt:-8],
                      [NSNumber numberWithInt:-9],
                      nil];
#if 0
  floats = [[NSSet alloc] initWithObjects:
            [NSNumber numberWithFloat:0.0],
            [NSNumber numberWithFloat:1.2],
            [NSNumber numberWithFloat:-0.2],
            [NSNumber numberWithFloat:1543432.0],
            [NSNumber numberWithFloat:-5435.00],
            [NSNumber numberWithFloat:9.7],
            [NSNumber numberWithFloat:3.14159],
            [NSNumber numberWithFloat:2.718],
            [NSNumber numberWithFloat:100.4342],
            nil];
#endif
};

- (void)tearDown {
  [checkNumberEquivalence release];
  
  [boundaries release];
  [signedIntegers release];
  [floats release];
  
  [super tearDown];
}

#define TEST_SET_WITH_BLOCK(set, block)                       \
- (void)test_ ## block ## _ ## set {                          \
    [set enumerateObjectsWithOptions:NSEnumerationConcurrent  \
                          usingBlock:block];                  \
}

TEST_SET_WITH_BLOCK(signedIntegers, checkNumberEquivalence)
//TEST_SET_WITH_BLOCK(floats, checkNumberEquivalence)
TEST_SET_WITH_BLOCK(boundaries, checkNumberEquivalence)

#if 0
- (void)testOverflowNumbers {
  STAssertNil([[LGGOTPNumber alloc] initWithNumber:
               [NSNumber numberWithLongLong:LGGO_MAX_UINT52+1]], nil);
  STAssertNil([[LGGOTPNumber alloc] initWithNumber:
               [NSNumber numberWithUnsignedLongLong:LGGO_MAX_UINT52+1]], nil);
  STAssertNil([[LGGOTPNumber alloc] initWithNumber:
               [NSNumber numberWithLongLong:LGGO_MIN_INT52-1]], nil);
  STAssertNil([[LGGOTPNumber alloc] initWithNumber:
               [NSNumber numberWithDouble:1.0]], nil);
}
#endif

@end
