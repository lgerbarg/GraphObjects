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


#ifndef LGGOCXXCLASS_H
#define LGGOCXXCLASS_H

#include <map>
#include <tr1/tuple>

#include "LGGOCXXType.h"

//FIXME we should probably cache property and relationship offsets

//Encoding and semantics
typedef std::tr1::tuple<LGGOCXXRelationEncodingType, LGGOCXXRelationSemanticsType> LGGOCXXRelationTuple;

class LGGOCXXClass : public LGGOCXXType {
private:
  LGGOCXXAddress address;
  std::string name;
  std::map<std::string, LGGOCXXScalarEncodingType> properties;
  std::map<std::string, LGGOCXXRelationTuple> relations;
  bool dirty:1;
public: 
  LGGOCXXClass(LGGOCXXSharedStoreContext C, std::string N);
  void addProperty(std::string name, LGGOCXXScalarEncodingType type);
  void addRelation(std::string name, LGGOCXXRelationEncodingType type, LGGOCXXRelationSemanticsType semantics);
  
  LGGOCXXScalarEncodingType getPropertyType(std::string name);
  uint32_t getPropertyOffset(std::string name);
  
  LGGOCXXRelationEncodingType getRelationType(std::string name);
  LGGOCXXRelationSemanticsType getRelationSemantics(std::string name);
  uint32_t getRelationOffset(std::string name);
  
  virtual LGGOCXXAddress getAddress(void);
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void);

  virtual bool isDirty(void);
  virtual bool isDusty(void);
};

#endif