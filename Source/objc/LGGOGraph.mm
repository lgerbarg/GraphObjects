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


#include "LGGOCXXStore.h"
#include "LGGOCXXStoreContext.h"

#import "LGGOGraphContext.h"

#import "LGGOGraph.h"

@interface LGGOGraph () {
  NSURL *URL;
  LGGOCXXSharedStore store;
}
@end

@implementation LGGOGraph

- (id)initWithURL:(NSURL *)URL_ {
  assert(URL_ != nil);
  
  if ((self = [super init])) {
    URL = [URL_ retain];
    store = LGGOCXXSharedStore(new LGGOCXXStore(URL.path.UTF8String));
  }
  
  return self;
}

- (void)dealloc {
  [URL release];
  
  [super dealloc];
}

- (LGGOGraphContext *)newContext {
  LGGOCXXSharedStoreContext context = LGGOCXXSharedStoreContext(store->newContext());
  LGGOGraphContext *retval =  [[LGGOGraphContext alloc] initWithCXXContext:context];
  return retval;
}

@end
