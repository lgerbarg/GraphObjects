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

#include "LGGOCXXStoreHash.h"


void LGGOCXXCoreStoreHash::debugPrint(void) {
  uint8_t i;
  const uint8_t * const digits = getHashPointer();
  
  for(i = 0; i < CC_SHA224_DIGEST_LENGTH; ++i) {
    printf("%02x", digits[i]);
    //std::cout << std::hex << getHashPointer()[i];
  }
  
  //std::cout << std::endl;
  printf("\n");
}

const std::string LGGOCXXCoreStoreHash::getHexValue (void) {
  std::ostringstream retval;
  const uint8_t * const digits = getHashPointer();
  uint8_t i;
  
  for(i = 0; i < CC_SHA224_DIGEST_LENGTH; ++i) {
    retval << std::hex << digits[i];
  }
  
  return retval.str();
}


bool LGGOCXXCoreStoreHash::operator< (LGGOCXXCoreStoreHash& b) {
  int result = memcmp(this->getHashPointer(), b.getHashPointer(), CC_SHA224_DIGEST_LENGTH);
  
  if (result < 0) {
    return true;
  } else {
    return false;
  }
}

bool LGGOCXXCoreStoreHash::operator> (LGGOCXXCoreStoreHash& b) {
  int result = memcmp(this->getHashPointer(), b.getHashPointer(), CC_SHA224_DIGEST_LENGTH);
  
  if (result > 0) {
    return true;
  } else {
    return false;
  }
}

bool LGGOCXXCoreStoreHash::operator== (LGGOCXXCoreStoreHash& b) {
  int result = memcmp(this->getHashPointer(), b.getHashPointer(), CC_SHA224_DIGEST_LENGTH);
  
  if (result == 0) {
    return true;
  } else {
    return false;
  }
}

LGGOCXXNonIntrusiveStoreHash::LGGOCXXNonIntrusiveStoreHash(void *data, uint64_t len) {
  (void)CC_SHA224(data, len, (unsigned char *)hash);
}

const uint8_t *const LGGOCXXNonIntrusiveStoreHash::getHashPointer(void) {
  return &hash[0];
}

LGGOCXXIntrusiveStoreHash::LGGOCXXIntrusiveStoreHash(const LGGOCXXSharedMemoryDescriptor &D, uint64_t O) : descripter(D), offset(O) {
  descripter->wire();
}

LGGOCXXIntrusiveStoreHash::~LGGOCXXIntrusiveStoreHash(void) {
  descripter->unwire();
}

const uint8_t * const LGGOCXXIntrusiveStoreHash::getHashPointer(void) {
  return (uint8_t *)(((uintptr_t)descripter->getData())+offset);
}
