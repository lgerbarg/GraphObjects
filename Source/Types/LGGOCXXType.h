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

virtual class LGGOCXXType;
typedef std::tr1::shared_ptr<LGGOCXXType> LGGOCXXSharedType;
typedef std::tr1::weak_ptr<LGGOCXXType> LGGOCXXWeakType;

#ifndef LGGOCXXTYPE_H
#define LGGOCXXTYPE_H

#include <string>

#include "LGGOCXXStoreContext.h"
#include "LGGOCXXMemoryDescriptor.h"

#include "LGGOCXXAddress.h"

//FIXME should we support _Bool/c++ bool encoding?

//This is the base type for all the C++ graph object types
//Includes books for mapping to native objects (handled in the native objects context)

typedef enum {
  kLGGOCXX8BitSignedNumberType = 0,
  kLGGOCXX8BitUnsignedNumberType = 1,
  kLGGOCXX16BitSignedNumberType = 2,
  kLGGOCXX16BitUnsignedNumberType = 3,
  kLGGOCXX32BitSignedNumberType = 4,
  kLGGOCXX32BitUnsignedNumberType = 5,
  kLGGOCXX64BitSignedNumberType = 6,
  kLGGOCXX64BitUnsignedNumberType = 7,
  kLGGOCXXFloatNumberType = 8,
  kLGGOCXXDoubleNumberType = 9,
} LGGOCXXScalarEncodingType;

typedef enum {
  kLGGOCXXClassReferenceType = 0,
  kLGGOCXXClassArrayType = 1,
  kLGGOCXXClassSetType = 2,
  kLGGOCXXClassDictionaryType = 3
} LGGOCXXRelationEncodingType;

typedef enum {
  kLGGOCXXClassByValueType = 0,
  kLGGOCXXClassByReferenceType = 1
} LGGOCXXRelationSemanticsType;

virtual class LGGOCXXType : public std::tr1::enable_shared_from_this<LGGOCXXType> {
private:
  LGGOCXXSharedStoreContext context;
  void *clientData;
public:
  LGGOCXXType(const LGGOCXXSharedStoreContext& C);
  LGGOCXXSharedStoreContext getContext(void);
  
  void *getNativeObject(void);
  void setNativeObject(void *D);
  
  virtual LGGOCXXAddress getAddress(void) = 0;
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void) = 0;
  virtual bool isDirty(void) = 0;
  virtual bool isDusty(void) = 0;
};

#endif