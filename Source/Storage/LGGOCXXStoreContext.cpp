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

#include <unistd.h>
#include <fcntl.h>
#include <vector>

#include <sstream>

#include "LGGOCXXType.h"
#include "LGGOCXXStore.h"
#include "LGGOCXXStoreSegment.h"
#include "LGGOCXXMemoryDescriptor.h"

#include "LGGOCXXStoreContext.h"

LGGOCXXStoreContext::LGGOCXXStoreContext(const LGGOCXXSharedStore &S)
  : store(S), nextAddressValue(256), nativeRetainFunc(NULL),  nativeReleaseFunc(NULL) {
  writableSegment = new LGGOCXXWritableStoreSegment;
}

LGGOCXXAddress LGGOCXXStoreContext::getNextFreeAddress (void) {
  LGGOCXXAddress retval = LGGOCXXAddress(nextAddressValue<<4);
  nextAddressValue++;
  return retval;
}

void LGGOCXXStoreContext::setNativeObjectRetain(void (*NRF)(void *)) {
  nativeRetainFunc = NRF;
}

void LGGOCXXStoreContext::setNativeObjectRelease(void (*NRF)(void *)) {
  nativeReleaseFunc = NRF;
}

void LGGOCXXStoreContext::nativeObjectRetain(void *nativeObject) {
  if (nativeRetainFunc) {
    nativeRetainFunc(nativeObject);
  }
}

void LGGOCXXStoreContext::nativeObjectRelease(void *nativeObject) {
  if (nativeReleaseFunc) {
    nativeReleaseFunc(nativeObject);
  }
}

//FIXME implement transient save here

void LGGOCXXStoreContext::writeSegment (void) {
  std::ostringstream path;
  
  int fd = open(store->getStorePath().c_str(), O_WRONLY | O_CREAT);
  LGGOCXXSharedMemoryDescriptor descriptor = writableSegment->serializeToMemory();
  path << store->getStorePath() << "/" << descriptor->getHash()->getHexValue();
  
  
  if (fd > 0) {
    write(fd, descriptor->getData(), descriptor->getSize());
    delete writableSegment;
    writableSegment = new LGGOCXXWritableStoreSegment;
    LGGOCXXSharedMemoryDescriptor newDescriptor = LGGOCXXSharedMemoryDescriptor(new LGGOCXXMappedMemoryDescriptor(path.str()));
    LGGOCXXSharedStoreSegment newSegment = LGGOCXXSharedStoreSegment(new LGGOCXXStoreSegment(newDescriptor));
    segments.push_back(newSegment);
  } else {
    assert(0);
  }
}

void LGGOCXXStoreContext::commit (void) {
  //FIXME we need to use a locking protocol here
}

void LGGOCXXStoreContext::setResolvedObjectForAddress(const LGGOCXXSharedType &object, LGGOCXXAddress address) {
  //resolvedObjects[address] = LGGOCXXWeakType(object);
  resolvedObjects[address] = object;
}

LGGOCXXSharedType LGGOCXXStoreContext::resolvedObjectForAddress(LGGOCXXAddress address) {
  LGGOCXXSharedType retval;
  
  std::map<LGGOCXXAddress,LGGOCXXSharedType>::iterator i;
  i = resolvedObjects.find(address);

  if (i != resolvedObjects.end()) {
    retval = i->second;
  }
  
  return retval;
}

LGGOCXXSharedType LGGOCXXStoreContext::rootObject(void) {
  return getObjectForAddress(LGGOCXXAddress(1<<4));
}

void LGGOCXXStoreContext::setRootObject(const LGGOCXXSharedType &object) {
  setResolvedObjectForAddress(object, LGGOCXXAddress(1<<4));
  setObjectForAddress(object, LGGOCXXAddress(1<<4));
}

void LGGOCXXStoreContext::setObjectForAddress(const LGGOCXXSharedType &object, LGGOCXXAddress address) {
  //FIXME this could be deferred
  writableSegment->setDescriptorForAddress(object->getSerializedData(), address);
}

LGGOCXXSharedType LGGOCXXStoreContext::getObjectForAddress (LGGOCXXAddress address) {
  LGGOCXXSharedType retval = resolvedObjectForAddress(address);
  
  if (retval.get() == NULL) {
    LGGOCXXSharedMemoryDescriptor descriptor = writableSegment->getDescriptorForAddress(address);
    
    if (descriptor.get() == NULL) {
    
      std::vector<LGGOCXXSharedStoreSegment>::iterator i;
      for (i = segments.begin(); i != segments.end(); ++i) {
        descriptor = writableSegment->getDescriptorForAddress(address);
        
        if (descriptor.get() != NULL) {
          break;
        }
      }
    }
  }
  
  return retval;
}