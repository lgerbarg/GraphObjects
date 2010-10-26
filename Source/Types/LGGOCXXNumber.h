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


#ifndef LGGOCXXNUMBER_H
#define LGGOCXXNUMBER_H

#include "LGGOCXXType.h"

// Encoding
// bits 0-3 number tag
// bits 4 subtype encoding (0 integer, 1 double) float
// bits 5-62 58 bit postiive integer (NOT 2's complement this is a magnitude)
// bit 63 sign bit

// float/double encodings are currently undefined
// Encoding that do not fit within a tagged pointer are currently not supported

typedef enum {
  LGGOCXXIntegerTypeEncoding = 0,
  LGGOCXXUnsignedFloatTypEncodinge = 1
} LGGOCXXNumberEncodingType;

class LGGOCXXNumber : public LGGOCXXType {
private:
  LGGOCXXSharedMemoryDescriptor descriptor;
  LGGOCXXAddress address;
  LGGOCXXScalarEncodingType type;
  bool dirty:1;
public:
  explicit LGGOCXXNumber(LGGOCXXSharedStoreContext C, int64_t N);
  
  LGGOCXXScalarEncodingType getType(void);
  int64_t signedValue (void);
  uint64_t unsignedValue (void);
  float floatValue(void);
  double doubleValue(void);

  virtual LGGOCXXAddress getAddress(void);
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void);
  virtual bool isDirty(void);
  virtual bool isDusty(void);
};

#endif