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

#include "LGGOCXXNumber.h"

#define LGGO_NUMBER_DATAMASK ((uint64_t)0x07ffffffffffffff)

#define LGGO_MAX_INT59 ((int64_t)0x03ffffffffffffff)
#define LGGO_MIN_INT59 ((int64_t)(-1*LGGO_MAX_INT59))

LGGOCXXNumber::LGGOCXXNumber(LGGOCXXSharedStoreContext C, int64_t N) : LGGOCXXType(C), dirty(true), address(0) {
  if (N >= 0) {
    if (N <= LGGO_MAX_INT59) {
      address = LGGOCXXAddress(((uint64_t)kLGGOAddressNumberType) | ((uint64_t)LGGOCXXIntegerTypeEncoding << 4) | ((uint64_t)N << 5));
      
      if (N > UINT32_MAX) {
        type = kLGGOCXX64BitUnsignedNumberType;
      } else if (N > UINT16_MAX) {
        type = kLGGOCXX32BitUnsignedNumberType;
      } else if (N > UINT8_MAX) {
        type = kLGGOCXX16BitUnsignedNumberType;
      } else {
        type = kLGGOCXX8BitUnsignedNumberType;
      } 
      
    } else {
      assert(0 && "Deal with oversized ints");
    }
      
  } else {
    if (N >= LGGO_MIN_INT59) {
      //We do not use twos complement here because we don't always know how many bits our type is
      uint64_t absValue = -1*N;
      
      address = LGGOCXXAddress(kLGGOAddressNumberType | LGGOCXXIntegerTypeEncoding << 4 | ((uint64_t)absValue << 5) | ((uint64_t)0x1 << 63));
      
      if (N < INT32_MIN) {
        type = kLGGOCXX64BitSignedNumberType;
      } else if (N < INT16_MIN) {
        type = kLGGOCXX32BitSignedNumberType;
      } else if (N < INT16_MIN) {
        type = kLGGOCXX16BitSignedNumberType;
      } else {
        type = kLGGOCXX8BitSignedNumberType;
      } 
    } else {
      assert(0 && "Deal with oversized ints");
    }
  }
}

LGGOCXXScalarEncodingType LGGOCXXNumber::getType(void) {
  return type;
}

LGGOCXXAddress LGGOCXXNumber::getAddress(void) {
  //HACK
  //Remove this once we have native LGGOAddresses
  getContext()->setResolvedObjectForAddress(shared_from_this(), address);
  
  return address;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXNumber::getSerializedData(void) {
  return NULL_DESCRIPTOR;
}

bool LGGOCXXNumber::isDirty(void) {
  return dirty;
}

bool LGGOCXXNumber::isDusty(void) {
  return false;
}

int64_t LGGOCXXNumber::signedValue (void) {
  const uint64_t addressValue = address.getAddressValue();
  bool signBit = (uint64_t)addressValue >> 63;
  int64_t retval;
  
  uint64_t absValue = ((uint64_t)(addressValue & 0x7fffffffffffffff) >> 5);
  if (signBit) {
    retval = -1 * absValue;
  } else {
    retval = absValue;
  }
  return retval;
}

uint64_t LGGOCXXNumber::unsignedValue (void) {
  return  (address.getAddressValue() >> 5);
}

float LGGOCXXNumber::floatValue(void) {
  assert(0);
  return 0.0;
}

double LGGOCXXNumber::doubleValue(void) {
  assert(0);
  return 0.0;
}