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

#include <inttypes.h>

#import "LGGOGraph.h"
#import "LGGOGraphContext.h"

#import "LGGOMutableArray.h"

#import "LGGOArrayTests.h"

//Make this large enough to be a 3 level dense packed array
//#define ARRAY_COUNT (509*254+1)
//#define ARRAY_COUNT (509*1+1)
#define ARRAY_COUNT (255)

@implementation LGGOArrayTests

- (void) testCreation {
  LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
  LGGOGraphContext *context = [graph newContext];
  LGGOMutableArray *graphArray = [[LGGOMutableArray alloc] initInContext:context];
  NSMutableArray *array = [[NSMutableArray alloc] init];

  for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
    [array insertObject:[NSNumber numberWithUnsignedInteger:i] atIndex:0];
    [graphArray insertObject:[NSNumber numberWithUnsignedInteger:i] atIndex:0];
    STAssertEqualObjects(array, graphArray, nil);
  }

  STAssertEquals((NSUInteger)ARRAY_COUNT, graphArray.count, nil);
  
//  [self validateArrayTree:graphArray.rootNode];
  
	[graphArray release];
  [graph release];
	[array release];
}

- (void) testObjectReplacement {
  LGGOGraph *graph = [[LGGOGraph alloc] initWithURL:[NSURL URLWithString:@"file:///tmp/"]];
  LGGOGraphContext *context = [graph newContext];
  LGGOMutableArray *graphArray = [[LGGOMutableArray alloc] initInContext:context];
  NSMutableArray *array = [[NSMutableArray alloc] init];

  for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
    [array addObject:[NSNumber numberWithUnsignedInteger:i]];
    [graphArray addObject:[NSNumber numberWithUnsignedInteger:i]];
    STAssertEqualObjects(array, graphArray, nil);
  }
  
  for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
		STAssertEqualObjects([array objectAtIndex:i], [graphArray objectAtIndex:i], nil);
    
	}
  
  for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
    [array replaceObjectAtIndex:i withObject:[NSNumber numberWithUnsignedInteger:i+1]];
  }
  
  for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
    [graphArray replaceObjectAtIndex:i withObject:[NSNumber numberWithUnsignedInteger:i+1]];
  }
  
	for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
		STAssertEqualObjects([array objectAtIndex:i], [graphArray objectAtIndex:i], nil);

	}
  
  STAssertEquals((NSUInteger)ARRAY_COUNT, graphArray.count, nil);
  
	[graphArray release];
  [context release];
  [graph release];
	[array release];
}

#if 0
- (void) testLastObject {
	LGGOGraph *graph = [[LGGOGraph alloc] init];
  LGGOArray *graphArray = [[LGGOArray alloc] initWithGraph:graph];
	
	for(NSUInteger i = 0; i < ARRAY_COUNT; i++) {
//		NSNumber *currentNumber = [NSNumber numberWithUnsignedInteger:i];
//    [graphArray addObject:currentNumber];
//		STAssertEqualObjects(currentNumber, graphArray.lastObject, nil);
  }
	
	[graphArray release];
	[graph release];
}
#endif

@end