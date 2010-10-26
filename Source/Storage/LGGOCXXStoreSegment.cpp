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
#include <vector>
#include <tr1/tuple>

#include "LGGOByteOrder.h"

#include "LGGOCXXStoreHash.h"

#include "LGGOCXXStoreSegment.h"



//FIXME choose something better
#define SEGMENT_MAGIC (0xF000000FE000000E)

#define HASHTABLE_RECORD_SIZE (CC_SHA224_DIGEST_LENGTH+sizeof(uint64_t))

LGGOCXXStoreSegment::LGGOCXXStoreSegment(LGGOCXXSharedMemoryDescriptor D) : descriptor(D) {
  uint8_t *segmentData = (uint8_t *)descriptor->getData();
  
  uint64_t magic = LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[0]));
  assert(magic == SEGMENT_MAGIC);
  
  pointerCount = LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[16]));
  hashCount = LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[24]));
  pointerPointerTableOffset = 4*sizeof(uint64_t);
  pointerHashTableOffset = pointerPointerTableOffset + (pointerCount * sizeof(LGGOCXXAddress));
  hashHashTableOffset = pointerHashTableOffset + (pointerCount * CC_SHA224_DIGEST_LENGTH);
  hashDataTableOffset = hashHashTableOffset + (hashCount*HASHTABLE_RECORD_SIZE);
}



void LGGOCXXStoreSegment::setDescriptorForAddress(const LGGOCXXSharedMemoryDescriptor &objectt, LGGOCXXAddress address) {
  //Make this something real someday
  throw;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXStoreSegment::getDescriptorForAddress (LGGOCXXAddress address) {
  LGGOCXXSharedMemoryDescriptor retval = LGGOCXXSharedMemoryDescriptor((LGGOCXXMemoryDescriptor *)NULL);
  uint8_t *segmentData = (uint8_t *)descriptor->getData();
  
  uint64_t *pointerTablePointers = (uint64_t *)(&segmentData[pointerPointerTableOffset]);
//  char *pointerHashTable = (char *)(&segmentData[pointerHashTableOffset]);

  //FIXME make this a binary search
  uint64_t i;
  
  for (i = 0; i < pointerCount; ++i) {
    if (LGGOCXXAddress(pointerTablePointers[i]) == address) {
      break;
    }
  }
  
  
  //FIXME in the future use this with the shared hash infrastructure to save some memory
  LGGOCXXIntrusiveStoreHash hash = LGGOCXXIntrusiveStoreHash(descriptor, pointerHashTableOffset+(i*CC_SHA224_DIGEST_LENGTH));
  
  //FIXME make this a binary search
  for (i = 0; i < hashCount; ++i) {
    //FIX 32, it should be a constant, not magic
    LGGOCXXIntrusiveStoreHash potentialHash = LGGOCXXIntrusiveStoreHash(descriptor, hashHashTableOffset+(i*HASHTABLE_RECORD_SIZE));
    if (hash == potentialHash) {
      uint64_t objectHashOffset =
        LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[hashHashTableOffset+(i*HASHTABLE_RECORD_SIZE)+CC_SHA224_DIGEST_LENGTH]));
      uint64_t baseOffset = objectHashOffset + hashDataTableOffset;
      
      uint64_t objectSize = LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[baseOffset+0]));
      uint64_t compressedObjectSize = LGGOSwapLittleToHost64(*((uint64_t *)&segmentData[baseOffset+8]));
      uint64_t offset = baseOffset + 16;
      
      retval = LGGOCXXSharedMemoryDescriptor(new LGGOCXXCompressedChildMemoryDescriptor(descriptor, offset, compressedObjectSize, objectSize)); 
      break;
    }
  }

  return retval;
}


void LGGOCXXWritableStoreSegment::setDescriptorForAddress(const LGGOCXXSharedMemoryDescriptor &object, LGGOCXXAddress address) {
  memoryObjects[address] = object;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXWritableStoreSegment::getDescriptorForAddress (LGGOCXXAddress address) {
  LGGOCXXSharedMemoryDescriptor retval;
  std::map<LGGOCXXAddress,LGGOCXXSharedMemoryDescriptor>::iterator i = memoryObjects.find(address);
  
  if (i != memoryObjects.end()) {
    retval = i->second;
  }
  
  return retval;
}

LGGOCXXSharedMemoryDescriptor LGGOCXXWritableStoreSegment::serializeToMemory(void) {  
  std::map<LGGOCXXSharedStoreHash, LGGOCXXSharedMemoryDescriptor> hashMap;
  std::vector<std::tr1::tuple<uint64_t, uint64_t, LGGOCXXSharedMemoryDescriptor> > compressedDecriptors;
  std::map<LGGOCXXAddress,LGGOCXXSharedMemoryDescriptor>::iterator i;
  std::map<LGGOCXXSharedStoreHash, LGGOCXXSharedMemoryDescriptor>::iterator j;


  uint64_t k;

  uint64_t pointerCount = memoryObjects.size();
  uint64_t pointerTablePointersSize = sizeof(LGGOCXXAddress)*pointerCount;
  uint64_t *pointerTablePointers = (uint64_t *)malloc(pointerTablePointersSize);
  unsigned char *pointerTableHashes = (unsigned char *)malloc(CC_SHA224_DIGEST_LENGTH*sizeof(unsigned char)*pointerCount);

  //Walk through the pointer map and build the tables
  //We use two consecutive tables to improve data locality
  for (i = memoryObjects.begin(), k = 0; i != memoryObjects.end(); ++i, ++k) {
    //Build the pointer to hash table
    pointerTablePointers[k] = i->first.getConcreteAddressValue();
    LGGOCXXSharedMemoryDescriptor object = i->second;
    unsigned char * startPoint = &pointerTableHashes[k*CC_SHA224_DIGEST_LENGTH];
    LGGOCXXSharedStoreHash hash = object->getHash();
    bcopy(hash->getHashPointer(), startPoint, CC_SHA224_DIGEST_LENGTH);    
    
    //Stuff the objects into a hash table while forcing them to compress
    //FIXME test if this is a writable segment via a dynamic cast
    hashMap[hash] = object;
  }
  
  uint64_t currentOffset = 0;
  uint64_t totalCompressedObjectSize = 0;
  
  uint64_t hashCount = hashMap.size();
  uint8_t *hashTableHashes = (uint8_t *)malloc(HASHTABLE_RECORD_SIZE*hashCount);

  //Walk through and build the hash lookup table
  //We intermix the data because SHA224 is 24 bytes anyway, so adding 8 bytes makes in 32, preserving cacheline alignment
  for (j = hashMap.begin(), k = 0; j != hashMap.end(); ++j, ++k) {
    LGGOCXXSharedMemoryDescriptor compressedDescriptor = j->second->compressedDescriptor();
    uint64_t objectSize = j->second->getSize();
    uint64_t compressedObjectSize = compressedDescriptor->getSize();
    totalCompressedObjectSize += (compressedObjectSize + 2*sizeof(uint64_t));
    unsigned char * startPoint = &hashTableHashes[k*HASHTABLE_RECORD_SIZE];
    uint64_t *offset = ((uint64_t *)(&startPoint[CC_SHA224_DIGEST_LENGTH]));
    bcopy(j->first->getHashPointer(), startPoint, CC_SHA224_DIGEST_LENGTH);
    *offset = LGGOSwapHostToLittle64(currentOffset);
    currentOffset += (compressedObjectSize + 2*sizeof(uint64_t));
    compressedDecriptors.push_back(std::tr1::make_tuple(objectSize, compressedObjectSize, compressedDescriptor));
  }
  
  currentOffset = 0;
  uint8_t *hashTableObjects = (uint8_t *)malloc(totalCompressedObjectSize);
  
  std::vector<std::tr1::tuple<uint64_t, uint64_t, LGGOCXXSharedMemoryDescriptor> >::iterator l;
  
  //Walk through and copy the compressed descriptors into contiguous memory
  for (l = compressedDecriptors.begin(); l != compressedDecriptors.end(); ++l) {
    std::tr1::tuple<uint64_t, uint64_t, LGGOCXXSharedMemoryDescriptor> descriptorTuple = *l;
    uint64_t objectSize = std::tr1::get<0>(descriptorTuple);
    uint64_t compressedObjectSize = std::tr1::get<1>(descriptorTuple);
    LGGOCXXSharedMemoryDescriptor descriptor =  std::tr1::get<2>(descriptorTuple);
    
    assert(currentOffset + compressedObjectSize <= totalCompressedObjectSize);
    
    uint8_t * startPoint = &hashTableObjects[currentOffset];
    uint64_t *size = ((uint64_t *)(&startPoint[0]));
    uint64_t *compressedSize = ((uint64_t *)(&startPoint[8]));
    uint8_t *objectData = ((uint8_t *)(&startPoint[16]));

    *size = LGGOSwapHostToLittle64(objectSize);
    *compressedSize = LGGOSwapHostToLittle64(compressedObjectSize);
    bcopy(descriptor->getData(), objectData, compressedObjectSize);
    
    currentOffset = currentOffset + compressedObjectSize+2*sizeof(uint64_t);
  }

  //Make one large segment to lay it all out
  uint64_t totalSize = (4*sizeof(uint64_t))
    + pointerTablePointersSize
    + (CC_SHA224_DIGEST_LENGTH*sizeof(unsigned char)*pointerCount)
    + (HASHTABLE_RECORD_SIZE*hashCount)
    + totalCompressedObjectSize;
  
  uint8_t *segmentData = (uint8_t *)malloc(totalSize+1);
  
  //Write Magic
  
  //FIXME we need to cryptographically secure the pointer table here
  *((uint64_t *)(&segmentData[0])) = LGGOSwapHostToLittle64(SEGMENT_MAGIC);
  *((uint64_t *)(&segmentData[8])) = LGGOSwapHostToLittle64(0x0); //version and checksum will go here
  *((uint64_t *)(&segmentData[16])) = LGGOSwapHostToLittle64(pointerCount);
  *((uint64_t *)(&segmentData[24])) = LGGOSwapHostToLittle64(hashCount);
  
  currentOffset = 32;
  
  bcopy(pointerTablePointers, &segmentData[currentOffset], (sizeof(LGGOCXXAddress)*pointerCount));
  currentOffset = currentOffset + (sizeof(LGGOCXXAddress)*pointerCount);
  free(pointerTablePointers);

  
  bcopy(pointerTableHashes, &segmentData[currentOffset], (CC_SHA224_DIGEST_LENGTH*sizeof(unsigned char)*pointerCount));
  currentOffset = currentOffset + (CC_SHA224_DIGEST_LENGTH*sizeof(unsigned char)*pointerCount);
  free(pointerTableHashes);
  
  bcopy(hashTableHashes, &segmentData[currentOffset], HASHTABLE_RECORD_SIZE*hashCount);
  currentOffset = currentOffset + (HASHTABLE_RECORD_SIZE*hashCount);
  free(hashTableHashes);
 
  bcopy(hashTableObjects, &segmentData[currentOffset], totalCompressedObjectSize);
  free(hashTableObjects);
  
  return LGGOCXXSharedMemoryDescriptor(new LGGOCXXMemoryDescriptor(segmentData, totalSize, false, true));
}