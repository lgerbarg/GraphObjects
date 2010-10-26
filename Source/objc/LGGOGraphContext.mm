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

#include "LGGOCXXStoreContext.h"
#include "LGGOCXXString.h"
#include "LGGOCXXNumber.h"
#include "LGGOCXXHackArray.h"

#import "LGGOGraphContext.h"
#import "LGGOString.h"
#import "LGGONumber.h"
#import "LGGOMutableArray.h"

@protocol LGGOCXXSharedType

@property (nonatomic, readonly) LGGOCXXSharedType sharedType;

@end

@interface LGGOGraphContext () {
  LGGOCXXSharedStoreContext CXXContext;
}
@end 

@implementation LGGOGraphContext

- (id)initWithCXXContext:(const LGGOCXXSharedStoreContext&)context_  {
    if ((self = [super init])) {
      CXXContext = context_;
      CXXContext->setNativeObjectRetain((void (*)(void *))&CFRetain);
      CXXContext->setNativeObjectRelease((void (*)(void *))&CFRelease);
    }
    
    return self;
}

- (id) rootObject {
  return nil;
}

- (void)setRootObject:(id)rootObject {
  
}


// This function returns a graph object from a normal object. It will grab the underlying
// graph object, or make one (without an attached native object) if necessary.

- (id) transmuteToNativeObject:(LGGOCXXSharedType)graphObject {
  id retval = (id)graphObject->getNativeObject();
  
  if (!retval) {
    std::tr1::shared_ptr<LGGOCXXHackArray> lggoArray = std::tr1::dynamic_pointer_cast<LGGOCXXHackArray>(graphObject);
    if (lggoArray.get()) {
      return [[[LGGOMutableArray alloc] initWithGraphObject:graphObject inContext:self] autorelease];
    }
    
    std::tr1::shared_ptr<LGGOCXXString> lggoString = std::tr1::dynamic_pointer_cast<LGGOCXXString>(graphObject);
    if (lggoString.get()) {
      return [[[LGGOString alloc] initWithGraphObject:graphObject inContext:self] autorelease];
    }
    
    std::tr1::shared_ptr<LGGOCXXNumber> lggoNumber = std::tr1::dynamic_pointer_cast<LGGOCXXNumber>(graphObject);
    if (lggoNumber.get()) {
      return [[[LGGONumber alloc] initWithGraphObject:graphObject inContext:self] autorelease];
    }


  }
  
  return retval;
}

- (LGGOCXXSharedType) transmuteToGraphObject:(id)object {
  LGGOCXXSharedType retval;
  
  if ([object respondsToSelector:@selector(LGGOCXXSharedType)]) {
    id<LGGOCXXSharedType> lggoObject = object;
    retval = [lggoObject sharedType];
  } else if ([object isKindOfClass:[NSString class]]) {
    NSString *string = object;
    retval = LGGOCXXSharedType(new LGGOCXXString(CXXContext, string.UTF8String));
  } else if ([object isKindOfClass:[NSNumber class]]) {
    //FIXME this needs improvement for floats
    NSNumber *number = object;
    retval = LGGOCXXSharedType(new LGGOCXXNumber(CXXContext, number.longLongValue));
  } else if ([object isKindOfClass:[NSArray class]]) {
    NSArray *array = object;
    retval = LGGOCXXSharedType(new LGGOCXXHackArray(CXXContext));
    std::tr1::shared_ptr<LGGOCXXHackArray> lggoArray = std::tr1::dynamic_pointer_cast<LGGOCXXHackArray>(retval);
    [array enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
      lggoArray->addObject([self transmuteToGraphObject:obj]);
    }];
  } else {
    assert(0);
  }
  
  return retval;
}

@end
