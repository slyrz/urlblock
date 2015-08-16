#ifndef URLBLOCK_HASH_H
#define URLBLOCK_HASH_H

#include <stdint.h>
#include <stdlib.h>

#define hash32(data,len) spooky_hash32(data, len, 0x3e35c3e7)
#define hash64(data,len) spooky_hash64(data, len, 0x3e35c3e7490c8f65ll)

uint32_t spooky_hash32 (const void *data, size_t len, uint32_t seed);
uint64_t spooky_hash64 (const void *data, size_t len, uint64_t seed);
void spooky_hash128 (const void *data, size_t len, uint64_t * hash1, uint64_t * hash2);


#endif
