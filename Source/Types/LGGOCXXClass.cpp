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


#include "LGGOCXXClass.h"

LGGOCXXClass::LGGOCXXClass(LGGOCXXSharedStoreContext C, std::string N) : LGGOCXXType(C), name(N), dirty(true) {

}

LGGOCXXAddress LGGOCXXClass::getAddress (void) {
  if (address.isValid()) {
    address = getContext()->getNextFreeAddress();
    //HACK
    //Remove this once we have native LGGOAddresses
    getContext()->setResolvedObjectForAddress(shared_from_this(), address);
  }
  
  return address;
}

void LGGOCXXClass::addProperty(std::string name, LGGOCXXScalarEncodingType type) {
  assert(relations.count(name) == 0); //While we can structurally support scheme style duality, ObjC can't
  properties[name] = type;
}

void LGGOCXXClass::addRelation(std::string name, LGGOCXXRelationEncodingType type, LGGOCXXRelationSemanticsType semantics) {
  assert(properties.count(name) == 0); //While we can structurally support scheme style duality, ObjC can't
  relations[name] = LGGOCXXRelationTuple(type, semantics);
}

LGGOCXXScalarEncodingType LGGOCXXClass::getPropertyType(std::string name) {
  assert(properties.count(name) == 1);
  return properties[name];
}

//FIXME this should be somewhere else
static
uint8_t typeSize (LGGOCXXScalarEncodingType type) {
  switch (type) {
    case kLGGOCXX8BitSignedNumberType:
    case kLGGOCXX8BitUnsignedNumberType:
      return 1;
    case kLGGOCXX16BitSignedNumberType:
    case kLGGOCXX16BitUnsignedNumberType:
      return 2;
    case kLGGOCXX32BitSignedNumberType:
    case kLGGOCXX32BitUnsignedNumberType:
    case kLGGOCXXFloatNumberType:
      return 4;
    case kLGGOCXX64BitSignedNumberType:
    case kLGGOCXX64BitUnsignedNumberType:
    case kLGGOCXXDoubleNumberType:
      return 8;
  }
}

uint32_t LGGOCXXClass::getPropertyOffset(std::string name) {
  assert(properties.count(name) == 1);
  
  LGGOCXXScalarEncodingType type = properties[name];
  uint8_t tSize = ::typeSize(type);

  //Properties come after relations
  uint32_t retval = sizeof(LGGOCXXAddress)*relations.size();
  
  std::map<std::string, LGGOCXXScalarEncodingType>::iterator i;
    
  for (i = properties.begin(); i != properties.end(); ++i) {
    uint8_t iSize = ::typeSize(i->second);
    
    if (i->first == name) {
      //Found it, lets get out of here
      break;
    }
    
    if (iSize >= tSize) {
      retval += iSize;
    }
  }
  
  return retval;
}

LGGOCXXRelationEncodingType LGGOCXXClass::getRelationType(std::string name) {
  assert(relations.count(name) == 1);
  return std::tr1::get<0>(relations[name]);
}

LGGOCXXRelationSemanticsType LGGOCXXClass::getRelationSemantics(std::string name) {
  assert(relations.count(name) == 1);
  return std::tr1::get<1>(relations[name]);
}

uint32_t LGGOCXXClass::getRelationOffset(std::string name) {
  assert(relations.count(name) == 1);
  uint32_t retval = 0;
  
  std::map<std::string, LGGOCXXRelationTuple>::iterator i;
  
  for (i = relations.begin(); i != relations.end(); ++i) {    
    if (i->first == name) {
      //Found it, lets get out of here
      break;
    }
    
    retval += 8;
  }
  
  return retval;
}


LGGOCXXSharedMemoryDescriptor LGGOCXXClass::getSerializedData (void) {
  return NULL_DESCRIPTOR;
}

bool LGGOCXXClass::isDusty (void) {
  return false;
}

bool LGGOCXXClass::isDirty (void) {
  return dirty;
}