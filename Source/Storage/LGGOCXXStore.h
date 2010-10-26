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

// This is the story API which vends out contexts. In the future it will deal
// with cross context communication.

#include <tr1/memory>

class LGGOCXXStore;
typedef std::tr1::shared_ptr<LGGOCXXStore> LGGOCXXSharedStore;
typedef std::tr1::weak_ptr<LGGOCXXStore> LGGOCXXWeakStore;

#ifndef LGGOCXXSTORE_H
#define LGGOCXXSTORE_H

#include <string>

#include "LGGOCXXStoreContext.h"

class LGGOCXXStore : public std::tr1::enable_shared_from_this<LGGOCXXStore> {
private:
  std::string storePath;
public:
  explicit LGGOCXXStore(std::string SP);
  LGGOCXXStoreContext *newContext(void);
  const std::string getStorePath (void);
};


#endif