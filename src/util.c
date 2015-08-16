#include "util.h"

static long
msb (long x)
{
  long r = 0;
  while (x) {
    r++;
    x >>= 1;
  }
  return r;
}

static long
nearpow2 (long x)
{
  long m = msb (x);
  long h = 1 << m;
  long l = h >> 1;
  if ((x - l) < (h - x))
    return l;
  else
    return h;
}

size_t
optsize (size_t elements, double load)
{
  return nearpow2 ((long) ((double) elements / load));
}
