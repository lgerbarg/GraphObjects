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

// FIXME
// In order to avoid lookups and solve cycles this will need to become our core custom smart pointer class

#include "LGGOCXXType.h"

#ifndef LGGOADDRESS_H
#define LGGOADDRESS_H

typedef enum {
  kLGGOAddressNativeType = 0,
  kLGGOAddressConstantType = 1,
  kLGGOAddressNumberType = 3,
  kLGGOAddressStringType = 4,
  kLGGOAddressDateType = 5,
  kLGGOAddressArrayType = 6,
  kLGGOAddressComplexType = 15
} LGGOSimpleType;

#include <stdint.h>

//FIXME we can be stupid clever here and half the size by using a union, but it is tricky

class LGGOCXXAddress {
private:
  uint64_t address;
public:
  LGGOCXXAddress(const LGGOCXXAddress& A) : address(A.address) { }
  explicit LGGOCXXAddress(uint64_t A = 0) : address(A) {}
  const uint64_t getAddressValue (void) { return address; }
  
  const LGGOSimpleType getType (void) const { return static_cast<LGGOSimpleType>(address | (uint64_t)0x00ff); }
  const bool isNative (void) const { return ((address | (uint64_t)0x00ff) == kLGGOAddressNativeType); }
  const bool isValid (void) const { return address != 0; }
  
  
  uint64_t getConcreteAddressValue (void) const { return address; }
  
  bool operator== (const LGGOCXXAddress& A) {
    return (address == A.address);
  }
  
  bool operator> (const LGGOCXXAddress& b) const {
    return address > b.address;
  }
  
  bool operator< (const LGGOCXXAddress& b) const {
    return address < b.address;
  }
};

#endif