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


#include <stdint.h>

#if defined(__LITTLE_ENDIAN__)

static inline
uint16_t LGGOSwapHostToLittle16(uint16_t data) {
  return data;
}

static inline 
uint32_t LGGOSwapHostToLittle32(uint32_t data) {
  return data;
}

static inline 
uint64_t LGGOSwapHostToLittle64(uint64_t data) {
  return data;
}

static inline
uint16_t LGGOSwapLittleToHost16(uint16_t data) {
  return data;
}

static inline 
uint32_t LGGOSwapLittleToHost32(uint32_t data) {
  return data;
}

static inline 
uint64_t LGGOSwapLittleToHost64(uint64_t data) {
  return data;
}

#elif

static inline
uint16_t LGGOSwapHostToLittle16(uint16_t data) {
  return ((data << 8) | (data >> 8));
}

static inline 
uint32_t LGGOSwapHostToLittle32(uint32_t data) {
  return __builtin_bswap32(data);
}

static inline 
uint64_t LGGOSwapHostToLittle64(uint64_t data) {
  return __builtin_bswap64(data);
}

static inline
uint16_t LGGOSwapLittleToHost16(uint16_t data) {
  return ((data << 8) | (data >> 8));
}

static inline 
uint32_t LGGOSwapLittleToHost32(uint32_t data) {
  return __builtin_bswap32(data);
}

static inline 
uint64_t LGGOSwapLittleToHost64(uint64_t data) {
  return __builtin_bswap64(data);
}


#endif