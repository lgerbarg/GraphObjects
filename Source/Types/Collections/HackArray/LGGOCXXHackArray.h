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

#include <vector>

#include "LGGOCXXType.h"

class LGGOCXXObject;

//FIXME this should really be an STL compatible interface

// No point in documenting the encoding format
// 1) It is not implemented
// 2) This whole class will go away

class LGGOCXXHackArray : public LGGOCXXType {
private:
  LGGOCXXAddress address;
  std::vector<LGGOCXXAddress> objects;
  bool dirty:1;
public:
  LGGOCXXHackArray(LGGOCXXSharedStoreContext C) : LGGOCXXType(C), address(0), dirty(true) { }
  
  uint64_t getCount(void);
  LGGOCXXSharedType getObjectAtIndex(uint64_t i);
  
  void addObject(const LGGOCXXSharedType& object);
  void insertObjectAtIndex(const LGGOCXXSharedType& object, uint64_t index);
  void removeLastObject (void);
  void removeObjectAtIndex(uint64_t index);
  void replaceObjectAtIndexWithObject(const LGGOCXXSharedType& object, uint64_t index);
  
  virtual LGGOCXXAddress getAddress(void);
  virtual LGGOCXXSharedMemoryDescriptor getSerializedData (void);
  virtual bool isDirty(void);
  virtual bool isDusty(void);
};