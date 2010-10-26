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


#include "LGGOCXXString.h"

#import "LGGOGraphContext.h"
#import "LGGOString.h"


@interface LGGOString () {
  LGGOCXXSharedType sharedType;
  LGGOGraphContext *graphContext;
}

@property LGGOCXXSharedType sharedType;

@end

@implementation LGGOString

- (id) initWithString:(NSString *)string_ inContext:(LGGOGraphContext *)context_ {
  self = [super init];
	
  if (self) {
    sharedType = LGGOCXXSharedType(new LGGOCXXString(context_.CXXContext, std::string(string_.UTF8String)));
    graphContext = [context_ retain];
    sharedType->setNativeObject(self);
  }
  
  return self;
}

- (id)initWithGraphObject:(const LGGOCXXSharedType &)graphObject inContext:(LGGOGraphContext *)context_ {
  self = [super init];
  
  if (self) {
    sharedType = graphObject;
    graphContext = [context_ retain];
    sharedType->setNativeObject(self);
  }
  
  return self;
}

- (void) dealloc {
  [graphContext release];
  
  [super dealloc];
}

- (NSUInteger)length {
  NSUInteger retval = std::tr1::dynamic_pointer_cast<LGGOCXXString>(sharedType)->getLength();
  
  return retval;
}

- (unichar)characterAtIndex:(NSUInteger)index {
  return std::tr1::dynamic_pointer_cast<LGGOCXXString>(sharedType)->getCharacterAtIndex(index);
}

#if 0
- (void)getCharacters:(unichar *)buffer range:(NSRange)aRange {
  std::tr1::dynamic_pointer_cast<LGGOCXXString>(sharedType)->getCharsInRage(buffer, aRange.location, aRange.length);
}
#endif

@end
