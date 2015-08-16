#ifndef URLBLOCK_STRINGSET_H
#define URLBLOCK_STRINGSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct stringset {
  size_t size;
  char **buckets;
};

void stringset_init (struct stringset *, size_t);
void stringset_free (struct stringset *);
void stringset_add (struct stringset *, uint32_t, const char *, unsigned char);
int stringset_contains (struct stringset *, uint32_t, const char *, unsigned char *);
void stringset_dump (struct stringset *, FILE *, const char *, int);

#endif
