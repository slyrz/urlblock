#ifndef URLBLOCK_UTIL_H
#define URLBLOCK_UTIL_H

#include <stdlib.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define rol32(v,s) (((v) << (s)) | ((v) >> (32 - (s))))
#define rol64(v,s) (((v) << (s)) | ((v) >> (64 - (s))))

#define freenull(ptr) \
  do { \
    free (ptr); ptr = NULL; \
  } while (0)

size_t optsize (size_t, double);

#endif
