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


#include <assert.h>
#include <zlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <string>

#include "LGGOCXXMemoryDescriptor.h"

LGGOCXXSharedMemoryDescriptor NULL_DESCRIPTOR((LGGOCXXMemoryDescriptor *)NULL);

LGGOCXXMemoryDescriptor::LGGOCXXMemoryDescriptor(void *D, uint64_t S, bool C, bool F)
  : size(S), shouldFree(F), shouldCopy(C) {
  if (shouldCopy) {
    bcopy(D, data, size);
  } else {
    data = D;
  }
}

LGGOCXXMemoryDescriptor::~LGGOCXXMemoryDescriptor (void) {
  if (shouldFree) {
    free(data);
  }
}

LGGOCXXCoreMemoryDescriptor::iterator LGGOCXXCoreMemoryDescriptor::begin (void) {
  return static_cast<LGGOCXXMemoryDescriptor::iterator>(getData());
}

LGGOCXXCoreMemoryDescriptor::iterator LGGOCXXCoreMemoryDescriptor::end (void) {
  return static_cast<LGGOCXXMemoryDescriptor::iterator>(getData())+getSize();
}

LGGOCXXCoreMemoryDescriptor::value_type & LGGOCXXCoreMemoryDescriptor::operator[](uint64_t i) {
  uint8_t *data = (uint8_t *)getData();
  return data[i];
}

LGGOCXXSharedStoreHash LGGOCXXCoreMemoryDescriptor::getHash (void) {
  if (hash == NULL) {
    hash = LGGOCXXSharedStoreHash(new LGGOCXXNonIntrusiveStoreHash(getData(), getSize()));
  }
  
  return hash;
}

void LGGOCXXCoreMemoryDescriptor::debugPrint(void) {
  uint32_t i;
  uint8_t *data = (uint8_t *)getData();
  
  for(i = 0; i < getSize(); ++i) {
    if (i%32 == 0) {
      printf("0x%08x:\t", i);
    } else if (i%4 == 0) {
      printf(" ");
    }
    
    printf("%02x", data[i]);
    
    if (i%32 == 31) {
      printf("\n");
    }
  }
  
  printf("\n");
}

//FIXME these must be made atomic when we add support for sharing threads between descriptors
void LGGOCXXCoreMemoryDescriptor::wire(void) {
  assert(wireCount < UINT32_MAX);
  wireCount++;
}

void LGGOCXXCoreMemoryDescriptor::unwire(void) {
  assert(wireCount > 0);
  wireCount--;
}


void *LGGOCXXMemoryDescriptor::getData (void) {
  return data;
}

const uint64_t LGGOCXXMemoryDescriptor::getSize (void) {
  return size;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXCoreMemoryDescriptor::compressedDescriptor (void) {
  bool heapAllocated = false;
  z_stream strm;
  strm.next_in = (unsigned char *)getData();
  strm.avail_in = getSize();
  
  strm.data_type = Z_BINARY;
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
  
  uint64_t bufferSize = deflateBound(&strm, getSize());
  unsigned char *buffer = (unsigned char *)alloca(bufferSize);
  if (!buffer) {
    buffer = (unsigned char *)malloc(bufferSize);
    heapAllocated = true;
  }
  
  strm.avail_out = bufferSize;
  strm.next_out = buffer;
  
  //FIXME runtime check this instead of asserts
  deflateInit(&strm, 0);
  int err = deflate(&strm, Z_FINISH);
  assert(err == Z_STREAM_END);
  deflateEnd(&strm);
  
  if (heapAllocated) {
    //FIXME check to see if we should resize here
  } else {
    unsigned char *tempBuffer = buffer;
    buffer = (unsigned char *)malloc(strm.total_out);
    bcopy(tempBuffer, buffer, strm.total_out);
  }
  
  return LGGOCXXSharedMemoryDescriptor(new LGGOCXXMemoryDescriptor(buffer, strm.total_out, false, true));
}

LGGOCXXCompressedChildMemoryDescriptor::LGGOCXXCompressedChildMemoryDescriptor(LGGOCXXSharedMemoryDescriptor PD, uint64_t O, uint64_t CS, uint64_t S)
: parentDescriptor(PD), offset(O), compressedSize(CS), size(S), data(NULL) {
  
}

void *LGGOCXXCompressedChildMemoryDescriptor::getData (void) {
  if (data == NULL) {
    z_stream strm;
    
    data = malloc(size);
    
    strm.next_in = (unsigned char *)(((uintptr_t)parentDescriptor->getData())+offset);
    strm.avail_in = compressedSize;
    
    strm.data_type = Z_BINARY;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;
    
    strm.avail_out = size;
    strm.next_out = (unsigned char *)data;
    
    inflateInit(&strm);
    int err = inflate(&strm, Z_FINISH);
    inflateEnd(&strm);

    assert(err == Z_STREAM_END);
  }
  return data;
}

const uint64_t LGGOCXXCompressedChildMemoryDescriptor::getSize (void) {
  return size;
}

LGGOCXXMappedMemoryDescriptor::LGGOCXXMappedMemoryDescriptor(const std::string &path) {
  int fd = open(path.c_str(), O_RDONLY);

  if (fd > 0) {
    struct stat buf;
    int err = fstat(fd, &buf);
    
    if (err == 0) {
      size = buf.st_size;
      data = mmap(NULL, size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
      
      if (data == MAP_FAILED) {
        assert(0);
      }
    } else {
      assert(0);
    }
    
    close(fd);
  }
}

LGGOCXXMappedMemoryDescriptor::~LGGOCXXMappedMemoryDescriptor(void) {
  munmap(data, size);
}

void *LGGOCXXMappedMemoryDescriptor::getData (void) {
  return NULL;
}

const uint64_t LGGOCXXMappedMemoryDescriptor::getSize (void) {
  return 0;
}

LGGOCXXTaggedMemoryDescriptor::LGGOCXXTaggedMemoryDescriptor(LGGOCXXAddress A) : address(A) { }
  
void *LGGOCXXTaggedMemoryDescriptor::getData (void) {
  return static_cast<void *>(&address);
};

const uint64_t LGGOCXXTaggedMemoryDescriptor::getSize (void) {
  return 8;
}