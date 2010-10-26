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

// A context consists of a cache of active objects, and a series of segments of encoded objects
// at any time it has one writable segment where it can serialize out its active objects, and
// a stack of immutable segments that have already been written. In the future we will be able
// to share immutable segments between multiple contexts

#include <tr1/memory>
class LGGOCXXStoreContext;
typedef std::tr1::shared_ptr<LGGOCXXStoreContext> LGGOCXXSharedStoreContext;
typedef std::tr1::weak_ptr<LGGOCXXStoreContext> LGGOCXXWeakStoreContext;

#ifndef LGGOCXXSTORECONTEXT_H
#define LGGOCXXSTORECONTEXT_H

#include <vector>

#include "LGGOCXXStore.h"
#include "LGGOCXXStoreSegment.h"
#include "LGGOCXXType.h"
#include "LGGOCXXAddress.h"

class LGGOCXXWritableStoreSegment;

class LGGOCXXStoreContext : public std::tr1::enable_shared_from_this<LGGOCXXStoreContext> {
private:
  LGGOCXXSharedStore store;
  LGGOCXXWritableStoreSegment *writableSegment;
  std::vector<LGGOCXXSharedStoreSegment> segments;
  uint64_t nextAddressValue;
   void (*nativeReleaseFunc)(void *);
   void (*nativeRetainFunc)(void *);
  
  //FIXME this should be weak, but without native types that can bump shared_ptrs it needs to be strong for now
  std::map<LGGOCXXAddress,LGGOCXXSharedType> resolvedObjects;
public:
  explicit LGGOCXXStoreContext(const LGGOCXXSharedStore &S);
  LGGOCXXAddress getNextFreeAddress (void);
  void writeSegment (void);
  void commit (void);
  void setResolvedObjectForAddress(const LGGOCXXSharedType &object, LGGOCXXAddress address);
  LGGOCXXSharedType resolvedObjectForAddress(LGGOCXXAddress address);
  
  LGGOCXXSharedType getObjectForAddress (LGGOCXXAddress address);
  void setObjectForAddress(const LGGOCXXSharedType &object, LGGOCXXAddress address);  
  LGGOCXXSharedType rootObject(void);
  void setRootObject(const LGGOCXXSharedType &object);
  
  
  void setNativeObjectRetain(void (*NRF)(void *));
  void setNativeObjectRelease(void (*NRF)(void *));
  void nativeObjectRetain(void *nativeObject);
  void nativeObjectRelease(void *nativeObject);
  
};

#endif