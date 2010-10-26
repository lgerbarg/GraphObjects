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


#ifndef LGGOCXXSTRING_H
#define LGGOCXXSTRING_H

// Encoding
// bits 0-3 string tag
// bits 4-7 length in bytes (NOT NULL terminated, if the string exceeds 7 bytes set to 0xff and
//   and length will be determined by length of descriptor
// bits 8+ NON-NULL terminated string

// This encoding has the benefit that strings less than 7 bytes can fit inside a tagged pointer,
// but are also valid in a memory buffer. That means we can use the same code path for them by
// using LGGOTaggedMemoryDescriptor, which wraps an address and presents it as a memory buffer.

#include <string>

#include "LGGOCXXType.h"

class LGGOCXXString : public LGGOCXXType {
private:
  LGGOCXXSharedMemoryDescriptor stringDescriptor;
  LGGOCXXAddress address;
  uint32_t charLength;
  uint32_t byteLength;
  uint8_t offset;
  bool lengthCalculated:1;
  bool dirty:1;
public:
  explicit LGGOCXXString(LGGOCXXSharedStoreContext C, std::string S);
  uint32_t getLength(void);
//  void getCharsInRage(uint16_t *buffer, uint32_t start, uint32_t len);
  uint16_t getCharacterAtIndex(uint32_t index);
  
  virtual LGGOCXXAddress getAddress(void);
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void);
  
  virtual bool isDirty(void);
  virtual bool isDusty(void);
};

#endif