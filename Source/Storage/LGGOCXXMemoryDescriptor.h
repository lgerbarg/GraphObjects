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

/* Memory descriptor are the core mechanism we use for tracking extents of ram.
   We can build them around mmaped files, in memory, etc. We can build one around
   an address in order to simplifiy classes that support tagged pointers
 */

#include <tr1/memory>
virtual class LGGOCXXCoreMemoryDescriptor;
typedef std::tr1::shared_ptr<LGGOCXXCoreMemoryDescriptor> LGGOCXXSharedMemoryDescriptor;

#ifndef LGGOCXXMEMORYDESCRIPTOR_H
#define LGGOCXXMEMORYDESCRIPTOR_H

extern LGGOCXXSharedMemoryDescriptor NULL_DESCRIPTOR;

#include <assert.h>

#include "LGGOCXXAddress.h"
#include "LGGOCXXStoreHash.h"

virtual class LGGOCXXCoreMemoryDescriptor : public std::tr1::enable_shared_from_this<LGGOCXXCoreMemoryDescriptor> {
private:
  LGGOCXXSharedStoreHash hash;
  uint32_t wireCount;
public:
  LGGOCXXSharedStoreHash getHash (void);
  
  LGGOCXXSharedMemoryDescriptor compressedDescriptor(void);
  
  void debugPrint(void);
  virtual void *getData (void) = 0;
  virtual const uint64_t getSize (void) = 0;
  
  //FIXME these must be made atomic when we add support for sharing threads between descriptors
  void wire(void);
  void unwire(void);
  
  //iterator support
  typedef uint8_t value_type;
  typedef uint8_t * iterator;
  
  iterator begin (void);
  iterator end (void);
  value_type &operator[](uint64_t i);
};

class LGGOCXXMemoryDescriptor : public LGGOCXXCoreMemoryDescriptor {
private:
  uint64_t size;
  void *data;
  bool shouldFree:1;
  bool shouldCopy:1;
  
public:
  LGGOCXXMemoryDescriptor(void *D, uint64_t S, bool C, bool F);
  ~LGGOCXXMemoryDescriptor (void);
  
  virtual void *getData (void);
  virtual const uint64_t getSize (void); 
};

class LGGOCXXCompressedChildMemoryDescriptor : public LGGOCXXCoreMemoryDescriptor {
private:
  uint64_t size;
  uint64_t compressedSize;
  uint64_t offset;
  void *data;
  LGGOCXXSharedMemoryDescriptor parentDescriptor;

public:
  LGGOCXXCompressedChildMemoryDescriptor(LGGOCXXSharedMemoryDescriptor PD, uint64_t O, uint64_t CS, uint64_t S);
  
  virtual void *getData (void);
  virtual const uint64_t getSize (void); 
};

class LGGOCXXMappedMemoryDescriptor : public LGGOCXXCoreMemoryDescriptor {
private:
  uint64_t size;
  void *data;
public:
  explicit LGGOCXXMappedMemoryDescriptor(const std::string &path);
  ~LGGOCXXMappedMemoryDescriptor(void);
  
  virtual void *getData (void);
  virtual const uint64_t getSize (void); 
};

class LGGOCXXTaggedMemoryDescriptor : public LGGOCXXCoreMemoryDescriptor {
private:
  LGGOCXXAddress address;
public:
  explicit LGGOCXXTaggedMemoryDescriptor(LGGOCXXAddress A);
  
  virtual void *getData (void);
  virtual const uint64_t getSize (void);
};

#endif