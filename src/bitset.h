#ifndef URLBLOCK_BITSET_H
#define URLBLOCK_BITSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct bitset {
  size_t size;
  uint32_t *buckets;
};

void bitset_init (struct bitset *, size_t);
void bitset_free (struct bitset *);
void bitset_add (struct bitset *, uint32_t);
int bitset_contains (struct bitset *, uint32_t);
void bitset_dump (struct bitset *, FILE *, const char *);

#endif
