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

#include <stdint.h>

#include "utf8.h"

LGGOCXXString::LGGOCXXString(LGGOCXXSharedStoreContext C, std::string S)
  : LGGOCXXType(C), charLength(0), lengthCalculated(false), dirty(true), address(0) {
  byteLength = S.size();

  if(byteLength <= 7) {
    uint32_t i;
    uint8_t *buffer = (uint8_t *)&address;
    buffer[0] = kLGGOAddressStringType | (uint8_t)byteLength << 4;
    
    for(i = 0; i < byteLength; ++i) {
      buffer[i+1] = S[i];
    }
    
    stringDescriptor = LGGOCXXSharedMemoryDescriptor(new LGGOCXXTaggedMemoryDescriptor(address));
  } else {
    uint8_t *buffer = static_cast<uint8_t *>(malloc(byteLength+1));
    buffer[0] = kLGGOAddressStringType | (uint8_t)0xff << 4;
    
    //FIXME check this
    memcpy (&buffer[1], S.data(), byteLength);
    stringDescriptor = LGGOCXXSharedMemoryDescriptor(new LGGOCXXMemoryDescriptor(buffer, byteLength+1, false, false));
  }
}

bool LGGOCXXString::isDirty(void) {
  return dirty;
}

bool LGGOCXXString::isDusty(void) {
  return false;
}

uint32_t LGGOCXXString::getLength(void) {
  if (lengthCalculated == false) {
    LGGOCXXCoreMemoryDescriptor::iterator i = stringDescriptor->begin()+1;

    while (i <= stringDescriptor->begin()+byteLength) {
      uint32_t codePoint = utf8::next(i, stringDescriptor->end());
      if (codePoint <=0xffff) {
        charLength += 1;
      } else {
        charLength += 2;
      }
    }
    lengthCalculated = true;
  }
  
  return charLength;
}

uint16_t LGGOCXXString::getCharacterAtIndex(uint32_t index) {
  uint16_t retval = 0;
  
  LGGOCXXCoreMemoryDescriptor::iterator i = stringDescriptor->begin()+1;
  uint32_t currentIndex = 0;
  
  while (i <= stringDescriptor->begin()+byteLength) {
    uint32_t codePoint = utf8::next(i, stringDescriptor->end());
    if (codePoint <=0xffff) {
      if (currentIndex == index) {
        retval = codePoint;
        break;
      } else {
        currentIndex += 1;
      }
    } else {
      if (currentIndex == index) {
        retval = (((codePoint)>>10)+0xd7c0);
        break;
      } else  if (currentIndex == index-1) {
        retval = (((codePoint)&0x3ff)|0xdc00);
        break;
      } else {
        currentIndex += 2;
      }
    }
    
  }
  
  return retval;
}

LGGOCXXAddress LGGOCXXString::getAddress(void) {
  if (!address.isValid()) {
    address = getContext()->getNextFreeAddress();
  }
  
  //HACK
  //Remove this once we have native LGGOAddresses
  getContext()->setResolvedObjectForAddress(shared_from_this(), address);
  
  return address;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXString::getSerializedData (void) {
  return stringDescriptor;
}