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

#import "LGGOCXXStoreSegmentTests.h"

#define TEST_OBJECT_COUNT (10 * 1024)

@implementation LGGOCXXStoreSegmentTests

- (void) testWritableSegmentCreation {
  LGGOCXXWritableStoreSegment *segment = new LGGOCXXWritableStoreSegment;
  STAssertTrue(segment != nil, nil);
  delete segment;
}

- (void) testWritableSegmentStorage {
  LGGOCXXWritableStoreSegment *segment = new LGGOCXXWritableStoreSegment;
  
  for(NSUInteger i = 0; i < TEST_OBJECT_COUNT; i++) {
    uint64_t len = i%4096;
    void *data = malloc(len);
    memset(data, i, len);
    LGGOCXXSharedMemoryDescriptor descriptor(new LGGOCXXMemoryDescriptor(data, len, false, true));
    
    segment->setDescriptorForAddress(descriptor, LGGOCXXAddress(i));
  }
  
  for(NSUInteger i = 0; i < TEST_OBJECT_COUNT; i++) {
    uint64_t len = i%4096;
    void *data = malloc(len);
    memset(data, i, len);
    
    LGGOCXXSharedMemoryDescriptor descriptor = segment->getDescriptorForAddress(LGGOCXXAddress(i));
    
    STAssertTrue(len == descriptor->getSize(), nil);
    STAssertTrue(!memcmp(data, descriptor->getData(), len), nil);
    
    free(data);
  }

  delete segment;
}

- (void) testSegmentSerialization {
  LGGOCXXWritableStoreSegment *segment = new LGGOCXXWritableStoreSegment;
  
  for(NSUInteger i = 0; i < TEST_OBJECT_COUNT; i++) {
    uint64_t len = i%4096;
    void *data = malloc(len);
    memset(data, i, len);
    LGGOCXXSharedMemoryDescriptor descriptor(new LGGOCXXMemoryDescriptor(data, len, false, true));
    
    segment->setDescriptorForAddress(descriptor, LGGOCXXAddress(i));
  }
  
  LGGOCXXSharedMemoryDescriptor serializedSegment = segment->serializeToMemory();
  delete segment;

  LGGOCXXSharedStoreSegment deserializedSegment = LGGOCXXSharedStoreSegment(new LGGOCXXStoreSegment(serializedSegment));

  for(NSUInteger i = 0; i < TEST_OBJECT_COUNT; i++) {
    uint64_t len = i%4096;
    void *data = malloc(len);
    memset(data, i, len);
    
    LGGOCXXSharedMemoryDescriptor descriptor = deserializedSegment->getDescriptorForAddress(LGGOCXXAddress(i));
    
    STAssertTrue(len == descriptor->getSize(), @"%llu != %llu", len, descriptor->getSize());
    STAssertTrue(!memcmp(data, descriptor->getData(), len), nil);
    
    free(data);
  }
}

@end
