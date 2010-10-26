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

#include "LGGOCXXHackArray.h"

uint64_t LGGOCXXHackArray::getCount(void) {
  return objects.size();
}

LGGOCXXSharedType LGGOCXXHackArray::getObjectAtIndex(uint64_t i) {
  LGGOCXXSharedType retval;
  
  assert(objects.size() > i);
  LGGOCXXAddress address = objects[i];
  if (address.isNative()) {
    //FIXME deal with this once addresses support native objects again
    assert(0);
  } else {
    retval = getContext()->resolvedObjectForAddress(address);
  }
  return retval;
}

LGGOCXXAddress LGGOCXXHackArray::getAddress(void) {
  if (!address.isValid()) {
    address = getContext()->getNextFreeAddress();
    //HACK
    //Remove this once we have native LGGOAddresses
    getContext()->setResolvedObjectForAddress(shared_from_this(), address);
  }
  return LGGOCXXAddress();
}

void LGGOCXXHackArray::addObject(const LGGOCXXSharedType& object) {
  objects.push_back(object->getAddress());
}

void LGGOCXXHackArray::insertObjectAtIndex(const LGGOCXXSharedType& object, uint64_t index) {
  std::vector<LGGOCXXAddress>::iterator i = objects.begin()+index;
  objects.insert(i, object->getAddress());
}

void LGGOCXXHackArray::removeLastObject (void) {
  objects.pop_back();
}

void LGGOCXXHackArray::removeObjectAtIndex(uint64_t index) {
  std::vector<LGGOCXXAddress>::iterator i = objects.begin()+index;
  objects.erase(i);
}

void LGGOCXXHackArray::replaceObjectAtIndexWithObject(const LGGOCXXSharedType& object, uint64_t index) {
  std::vector<LGGOCXXAddress>::iterator i = objects.begin()+index;
  objects[index] = object->getAddress();
}

LGGOCXXSharedMemoryDescriptor LGGOCXXHackArray::getSerializedData (void) {
  assert(0);
}

bool LGGOCXXHackArray::isDirty(void) {
  return dirty;
}

bool LGGOCXXHackArray::isDusty(void) {
  return false;
}