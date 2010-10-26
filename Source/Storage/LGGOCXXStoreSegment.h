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

// Segments can be thought of as files, though they also exist as memory
// They consist of a table that maps pointers to hashes, and another table
// that maps hashes to compressed objects.

// We will need to add a vacuum thread that can coalesce segments in order to
// reclaim space, and also to reduce bandwidth during sync.

#include <tr1/memory>

virtual class LGGOCXXBaseStoreSegment;
typedef std::tr1::shared_ptr<LGGOCXXBaseStoreSegment> LGGOCXXSharedStoreSegment;

#ifndef LGGOCXXSTORESEGMENT_H
#define LGGOCXXSTORESEGMENT_H

#include <stdlib.h>
#include <strings.h>

#include <map>

#include "LGGOCXXAddress.h"
#include "LGGOCXXMemoryDescriptor.h"

//FIXME we should seperate pointer and hash lookups to support split storage

virtual class LGGOCXXBaseStoreSegment : public std::tr1::enable_shared_from_this<LGGOCXXBaseStoreSegment> {
public:
  virtual void setDescriptorForAddress(const LGGOCXXSharedMemoryDescriptor &object, LGGOCXXAddress address) = 0;
  virtual LGGOCXXSharedMemoryDescriptor getDescriptorForAddress (LGGOCXXAddress address) = 0;
};

class LGGOCXXWritableStoreSegment : public LGGOCXXBaseStoreSegment {
private:
  std::map<LGGOCXXAddress, LGGOCXXSharedMemoryDescriptor> memoryObjects;  
protected:
public:    
  LGGOCXXSharedMemoryDescriptor serializeToMemory(void);
  
  virtual void setDescriptorForAddress(const LGGOCXXSharedMemoryDescriptor &object, LGGOCXXAddress address);  
  virtual LGGOCXXSharedMemoryDescriptor getDescriptorForAddress (LGGOCXXAddress address);
};

class LGGOCXXStoreSegment : public LGGOCXXBaseStoreSegment {
private:
  LGGOCXXSharedMemoryDescriptor descriptor;
  uint64_t pointerCount;
  uint64_t hashCount;
  uint64_t pointerPointerTableOffset;
  uint64_t pointerHashTableOffset;
  uint64_t hashHashTableOffset;
  uint64_t hashDataTableOffset;
public:
  explicit LGGOCXXStoreSegment(LGGOCXXSharedMemoryDescriptor D);
  
  virtual void setDescriptorForAddress(const LGGOCXXSharedMemoryDescriptor &object, LGGOCXXAddress address);  
  virtual LGGOCXXSharedMemoryDescriptor getDescriptorForAddress (LGGOCXXAddress address);
};

#endif