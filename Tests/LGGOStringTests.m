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

#import "LGGOString.h"

#import "LGGOStringTests.h"

@interface LGGOStringTests () {
  NSSet *oneByteStrings;
  NSSet *twoByteStrings;
  NSSet *fourByteStrings;
  NSSet *mixedOneAndTwoByteStrings;
  NSSet *mixedStrings;
  NSSet *untaggedStrings;
  
  void (^checkStringEquivalence)(id obj, BOOL *stop);
  void (^checkStringCharAccess)(id obj, BOOL *stop);
  void (^checkStringBulkAccess)(id obj, BOOL *stop);
}
@end

@implementation LGGOStringTests

- (void) setUp {
  [super setUp];

  /*
   This block just creates a tagged string from a string, and tests for
   equality. That is actually a fairly good test, since it exercises a lot of
   NSString's machinery. This test found a lot of byte encoding issues.
   */
  checkStringEquivalence = [^(id string, BOOL* stop){
    LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
    LGGOGraphContext *context = [graph newContext];
    
    LGGOString *testString = [[LGGOString alloc] initWithString:string inContext:context];
    STAssertEqualObjects(testString, string, nil);
    [testString release];
    
    [context release];
    [graph release];
  } copy];
  
  /*
   This test explicitly exercises the only two routines required for an
   NSString subclass, -length and -characterAtIndex:. I want to explicitly
   test them, since other methods will generally use them, but may choose to
   use other methods when subclasses provide them.
   */
  checkStringCharAccess = [^(id string, BOOL* stop){
    LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
    LGGOGraphContext *context = [graph newContext];
    LGGOString *testString = [[LGGOString alloc] initWithString:string inContext:context];

    NSUInteger i;
    NSUInteger length = [string length];
    for (i = 0; i < length; i++) {
      STAssertEquals([testString characterAtIndex:i],
                    [string characterAtIndex:i], nil);

    }
    [testString release];
    
    [context release];
    [graph release];
  } copy];
  
  /*
   This test explicitly exercises -getCharacters:range:. That method is not a
   required method for NSString subclasses, but it is an immense performance
   win, and we implement it. When it is present NSString detects it, and uses
   it in favor of of -characterAtIndex: in many instances, so we want to
   explicitly check both, since we can't be sure which one the various other
   string methods will call.
   */
  checkStringBulkAccess = [^(id string, BOOL* stop){
    LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
    LGGOGraphContext *context = [graph newContext];
    
    NSUInteger i = 0;
    NSUInteger j = 0;
    NSUInteger length = [string length];

    for (i = 1; i <= length; i++) {
      for (j = 0; j < i; j++) {
        LGGOString *testString = [[LGGOString alloc] initWithString:string inContext:context];
        NSRange range = NSMakeRange(j, i-j);
//        NSRange range = NSMakeRange(0, length);
        
        unichar *stringBuffer = alloca(sizeof(unichar)*length);
        unichar *testStringBuffer = alloca(sizeof(unichar)*length);
        
        [string getCharacters:stringBuffer range:range];
        [testString getCharacters:testStringBuffer range:range];
        
        STAssertTrue(!memcmp(stringBuffer, 
                             testStringBuffer,
                             sizeof(unichar)*(i-j)),
                     nil);
        
  //      free(stringBuffer);
    //    free(testStringBuffer);
  //      
        [testString release];
      }
    }
    [context release];
    [graph release];
  } copy];
  
  oneByteStrings = [[NSSet alloc] initWithObjects:
                    @"A",
                    @"AB",
                    @"ABC",
                    @"ABCD",
                    @"ABCDE",
                    @"ABCDEF",
                    @"ABCDEFG",
                    nil];
  twoByteStrings = [[NSSet alloc] initWithObjects:
                    @"\xC2\xA2",
                    @"\xC2\xA2\xC2\xA3",
                    @"\xC2\xA2\xC2\xA3\xC2\xA4",
                    nil];
  fourByteStrings = [[NSSet alloc] initWithObjects:
                     @"\xF0\xA4\xAD\xA2",
                     nil];
  mixedOneAndTwoByteStrings = [[NSSet alloc] initWithObjects:
                               (@"X\xC2\xA2"),
                               (@"X\xC2\xA2Y"),
                               (@"X\xC2\xA2\xC2\xA3"),
                               (@"X\xC2\xA2Y\xC2\xA3"),
                               (@"X\xC2\xA2Y\xC2\xA3Z"),
                               (@"X\xC2\xA2\xC2\xA3\xC2\xA4"),
                               (@"\xC2\xA2X\xC2\xA3\xC2\xA4"),
                               (@"\xC2\xA2\xC2\xA3X\xC2\xA4"),
                               (@"\xC2\xA2\xC2\xA3\xC2\xA4X"),
                               nil];
  mixedStrings = [[NSSet alloc] initWithObjects:
                  @"X\xF0\xA4\xAD\xA2",
                  @"XY\xF0\xA4\xAD\xA2",
                  @"XYZ\xF0\xA4\xAD\xA2",
                  @"\xF0\xA4\xAD\xA2X",
                  @"\xF0\xA4\xAD\xA2XY",
                  @"\xF0\xA4\xAD\xA2XYZ",
                  @"X\xF0\xA4\xAD\xA2Y",
                  @"X\xF0\xA4\xAD\xA2YZ",
                  @"XY\xF0\xA4\xAD\xA2Z",
                  @"\xF0\xA4\xAD\xA2",
                  @"\xC2\xA2\xF0\xA4\xAD\xA2",
                  @"\xF0\xA4\xAD\xA2\xC2\xA2",
                  @"X\xC2\xA2\xF0\xA4\xAD\xA2",
                  @"\xC2\xA2X\xF0\xA4\xAD\xA2",
                  @"\xF0\xA4\xAD\xA2X\xC2\xA2",
                  @"\xF0\xA4\xAD\xA2\xC2\xA2X",
                  @"\xC2\xA2\xF0\xA4\xAD\xA2X",
                  @"X\xF0\xA4\xAD\xA2\xC2\xA2",
                  nil];
  untaggedStrings = [[NSSet alloc] initWithObjects:
                  @"ABCDEGHIJKLMNOPQRSTUVWXYZ",
                  nil];
}

- (void) tearDown {
  [checkStringEquivalence release];
  [checkStringCharAccess release];
  [checkStringBulkAccess release];
  
  [oneByteStrings release];
  [twoByteStrings release];
  [fourByteStrings release];
  [mixedOneAndTwoByteStrings release];
  [mixedStrings release];
  [untaggedStrings release];

  [super tearDown];
}

#define TEST_SET_WITH_BLOCK(set, block)                     \
- (void)test_ ## block ## _ ## set {                        \
  [set enumerateObjectsWithOptions:NSEnumerationConcurrent  \
                        usingBlock:block];                  \
}

TEST_SET_WITH_BLOCK(oneByteStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(oneByteStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(oneByteStrings, checkStringBulkAccess)

TEST_SET_WITH_BLOCK(twoByteStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(twoByteStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(twoByteStrings, checkStringBulkAccess)

TEST_SET_WITH_BLOCK(fourByteStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(fourByteStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(fourByteStrings, checkStringBulkAccess)


TEST_SET_WITH_BLOCK(mixedOneAndTwoByteStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(mixedOneAndTwoByteStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(mixedOneAndTwoByteStrings, checkStringBulkAccess)

TEST_SET_WITH_BLOCK(mixedStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(mixedStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(mixedStrings, checkStringBulkAccess)

TEST_SET_WITH_BLOCK(untaggedStrings, checkStringEquivalence)
TEST_SET_WITH_BLOCK(untaggedStrings, checkStringCharAccess)
TEST_SET_WITH_BLOCK(untaggedStrings, checkStringBulkAccess)

- (void) testMalformedStringsEquality {
  //FIXME write some
}

- (void) testMalformedStringsCharAccess {
  //FIXME write some
}

- (void) testMalformedStringsBulkAccess {
  //FIXME write some
}

@end
