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

#include <tr1/memory>

// This is basically a wrapper around a SHA224 hash. There are variants to suppot
// and non-intrusive storage of the hash bytes.

virtual class LGGOCXXCoreStoreHash;
typedef std::tr1::shared_ptr<LGGOCXXCoreStoreHash> LGGOCXXSharedStoreHash;

#ifndef LGGOCXXSTOREHASH_H
#define LGGOCXXSTOREHASH_H

#include <CommonCrypto/CommonDigest.h>
#include <sstream>

#include "LGGOCXXMemoryDescriptor.h"

virtual class LGGOCXXCoreStoreHash : public std::tr1::enable_shared_from_this<LGGOCXXCoreStoreHash> {
public:
  virtual const uint8_t * const getHashPointer(void) = 0;
  
  void debugPrint(void);
  const std::string getHexValue (void);
  bool operator< (LGGOCXXCoreStoreHash& b);
  bool operator> (LGGOCXXCoreStoreHash& b);
  bool operator== (LGGOCXXCoreStoreHash& b);
};

//FIXME we should implement uniquing, and also descriptor offset specializations

class LGGOCXXNonIntrusiveStoreHash : public LGGOCXXCoreStoreHash {
private:
  unsigned char hash[CC_SHA224_DIGEST_LENGTH];
public:
  LGGOCXXNonIntrusiveStoreHash(void *data, uint64_t len);
  virtual const uint8_t *const getHashPointer(void);
};

class LGGOCXXIntrusiveStoreHash : public LGGOCXXCoreStoreHash {
private:
  uint64_t offset;
  LGGOCXXSharedMemoryDescriptor descripter;
public:
  LGGOCXXIntrusiveStoreHash(const LGGOCXXSharedMemoryDescriptor &D, uint64_t O);
  ~LGGOCXXIntrusiveStoreHash(void);
  virtual const uint8_t * const getHashPointer(void);
};

#endif