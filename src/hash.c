#include "config.h"
#include <memory.h>

#include "hash.h"
#include "util.h"

#define ALLOW_UNALIGNED_READS 1
#define SPOOKY_CONST      0xdeadbeefdeadbeefLL
#define SPOOKY_NUMVARS    12
#define SPOOKY_BLOCKSIZE  (8 * SPOOKY_NUMVARS)
#define SPOOKY_BUFSIZE    (2 * SPOOKY_BLOCKSIZE)

static inline void
mix (const uint64_t * data, uint64_t * s0, uint64_t * s1, uint64_t * s2,
     uint64_t * s3, uint64_t * s4, uint64_t * s5, uint64_t * s6, uint64_t * s7,
     uint64_t * s8, uint64_t * s9, uint64_t * s10, uint64_t * s11)
{
  *s0 += data[0];
  *s2 ^= *s10;
  *s11 ^= *s0;
  *s0 = rol64 (*s0, 11);
  *s11 += *s1;
  *s1 += data[1];
  *s3 ^= *s11;
  *s0 ^= *s1;
  *s1 = rol64 (*s1, 32);
  *s0 += *s2;
  *s2 += data[2];
  *s4 ^= *s0;
  *s1 ^= *s2;
  *s2 = rol64 (*s2, 43);
  *s1 += *s3;
  *s3 += data[3];
  *s5 ^= *s1;
  *s2 ^= *s3;
  *s3 = rol64 (*s3, 31);
  *s2 += *s4;
  *s4 += data[4];
  *s6 ^= *s2;
  *s3 ^= *s4;
  *s4 = rol64 (*s4, 17);
  *s3 += *s5;
  *s5 += data[5];
  *s7 ^= *s3;
  *s4 ^= *s5;
  *s5 = rol64 (*s5, 28);
  *s4 += *s6;
  *s6 += data[6];
  *s8 ^= *s4;
  *s5 ^= *s6;
  *s6 = rol64 (*s6, 39);
  *s5 += *s7;
  *s7 += data[7];
  *s9 ^= *s5;
  *s6 ^= *s7;
  *s7 = rol64 (*s7, 57);
  *s6 += *s8;
  *s8 += data[8];
  *s10 ^= *s6;
  *s7 ^= *s8;
  *s8 = rol64 (*s8, 55);
  *s7 += *s9;
  *s9 += data[9];
  *s11 ^= *s7;
  *s8 ^= *s9;
  *s9 = rol64 (*s9, 54);
  *s8 += *s10;
  *s10 += data[10];
  *s0 ^= *s8;
  *s9 ^= *s10;
  *s10 = rol64 (*s10, 22);
  *s9 += *s11;
  *s11 += data[11];
  *s1 ^= *s9;
  *s10 ^= *s11;
  *s11 = rol64 (*s11, 46);
  *s10 += *s0;
}

static inline void
endPartial (uint64_t * h0, uint64_t * h1, uint64_t * h2, uint64_t * h3,
            uint64_t * h4, uint64_t * h5, uint64_t * h6, uint64_t * h7,
            uint64_t * h8, uint64_t * h9, uint64_t * h10, uint64_t * h11)
{
  *h11 += *h1;
  *h2 ^= *h11;
  *h1 = rol64 (*h1, 44);
  *h0 += *h2;
  *h3 ^= *h0;
  *h2 = rol64 (*h2, 15);
  *h1 += *h3;
  *h4 ^= *h1;
  *h3 = rol64 (*h3, 34);
  *h2 += *h4;
  *h5 ^= *h2;
  *h4 = rol64 (*h4, 21);
  *h3 += *h5;
  *h6 ^= *h3;
  *h5 = rol64 (*h5, 38);
  *h4 += *h6;
  *h7 ^= *h4;
  *h6 = rol64 (*h6, 33);
  *h5 += *h7;
  *h8 ^= *h5;
  *h7 = rol64 (*h7, 10);
  *h6 += *h8;
  *h9 ^= *h6;
  *h8 = rol64 (*h8, 13);
  *h7 += *h9;
  *h10 ^= *h7;
  *h9 = rol64 (*h9, 38);
  *h8 += *h10;
  *h11 ^= *h8;
  *h10 = rol64 (*h10, 53);
  *h9 += *h11;
  *h0 ^= *h9;
  *h11 = rol64 (*h11, 42);
  *h10 += *h0;
  *h1 ^= *h10;
  *h0 = rol64 (*h0, 54);
}

static inline void
end (uint64_t * h0, uint64_t * h1, uint64_t * h2, uint64_t * h3, uint64_t * h4,
     uint64_t * h5, uint64_t * h6, uint64_t * h7, uint64_t * h8, uint64_t * h9,
     uint64_t * h10, uint64_t * h11)
{
  endPartial (h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
  endPartial (h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
  endPartial (h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11);
}

static inline void
short_mix (uint64_t * h0, uint64_t * h1, uint64_t * h2, uint64_t * h3)
{
  *h2 = rol64 (*h2, 50);
  *h2 += *h3;
  *h0 ^= *h2;
  *h3 = rol64 (*h3, 52);
  *h3 += *h0;
  *h1 ^= *h3;
  *h0 = rol64 (*h0, 30);
  *h0 += *h1;
  *h2 ^= *h0;
  *h1 = rol64 (*h1, 41);
  *h1 += *h2;
  *h3 ^= *h1;
  *h2 = rol64 (*h2, 54);
  *h2 += *h3;
  *h0 ^= *h2;
  *h3 = rol64 (*h3, 48);
  *h3 += *h0;
  *h1 ^= *h3;
  *h0 = rol64 (*h0, 38);
  *h0 += *h1;
  *h2 ^= *h0;
  *h1 = rol64 (*h1, 37);
  *h1 += *h2;
  *h3 ^= *h1;
  *h2 = rol64 (*h2, 62);
  *h2 += *h3;
  *h0 ^= *h2;
  *h3 = rol64 (*h3, 34);
  *h3 += *h0;
  *h1 ^= *h3;
  *h0 = rol64 (*h0, 5);
  *h0 += *h1;
  *h2 ^= *h0;
  *h1 = rol64 (*h1, 36);
  *h1 += *h2;
  *h3 ^= *h1;
}

static inline void
short_end (uint64_t * h0, uint64_t * h1, uint64_t * h2, uint64_t * h3)
{
  *h3 ^= *h2;
  *h2 = rol64 (*h2, 15);
  *h3 += *h2;
  *h0 ^= *h3;
  *h3 = rol64 (*h3, 52);
  *h0 += *h3;
  *h1 ^= *h0;
  *h0 = rol64 (*h0, 26);
  *h1 += *h0;
  *h2 ^= *h1;
  *h1 = rol64 (*h1, 51);
  *h2 += *h1;
  *h3 ^= *h2;
  *h2 = rol64 (*h2, 28);
  *h3 += *h2;
  *h0 ^= *h3;
  *h3 = rol64 (*h3, 9);
  *h0 += *h3;
  *h1 ^= *h0;
  *h0 = rol64 (*h0, 47);
  *h1 += *h0;
  *h2 ^= *h1;
  *h1 = rol64 (*h1, 54);
  *h2 += *h1;
  *h3 ^= *h2;
  *h2 = rol64 (*h2, 32);
  *h3 += *h2;
  *h0 ^= *h3;
  *h3 = rol64 (*h3, 25);
  *h0 += *h3;
  *h1 ^= *h0;
  *h0 = rol64 (*h0, 63);
  *h1 += *h0;
}

static void
spooky_shorthash (const void *data, size_t len, uint64_t * hash1, uint64_t * hash2)
{
  uint64_t buf[2 * SPOOKY_NUMVARS];
  union {
    const uint8_t *p8;
    uint32_t *p32;
    uint64_t *p64;
    size_t i;
  } u;
  size_t remainder;
  uint64_t a, b, c, d;
  u.p8 = (const uint8_t *) data;

  if (!ALLOW_UNALIGNED_READS && (u.i & 0x7)) {
    memcpy (buf, data, len);
    u.p64 = buf;
  }

  remainder = len % 32;
  a = *hash1;
  b = *hash2;
  c = SPOOKY_CONST;
  d = SPOOKY_CONST;

  if (len > 15) {
    const uint64_t *endp = u.p64 + (len / 32) * 4;
    for (; u.p64 < endp; u.p64 += 4) {
      c += u.p64[0];
      d += u.p64[1];
      short_mix (&a, &b, &c, &d);
      a += u.p64[2];
      b += u.p64[3];
    }
    if (remainder >= 16) {
      c += u.p64[0];
      d += u.p64[1];
      short_mix (&a, &b, &c, &d);
      u.p64 += 2;
      remainder -= 16;
    }
  }
  d = ((uint64_t) len) << 56;
  switch (remainder) {
    case 15:
      d += ((uint64_t) u.p8[14]) << 48;
    case 14:
      d += ((uint64_t) u.p8[13]) << 40;
    case 13:
      d += ((uint64_t) u.p8[12]) << 32;
    case 12:
      d += u.p32[2];
      c += u.p64[0];
      break;
    case 11:
      d += ((uint64_t) u.p8[10]) << 16;
    case 10:
      d += ((uint64_t) u.p8[9]) << 8;
    case 9:
      d += (uint64_t) u.p8[8];
    case 8:
      c += u.p64[0];
      break;
    case 7:
      c += ((uint64_t) u.p8[6]) << 48;
    case 6:
      c += ((uint64_t) u.p8[5]) << 40;
    case 5:
      c += ((uint64_t) u.p8[4]) << 32;
    case 4:
      c += u.p32[0];
      break;
    case 3:
      c += ((uint64_t) u.p8[2]) << 16;
    case 2:
      c += ((uint64_t) u.p8[1]) << 8;
    case 1:
      c += (uint64_t) u.p8[0];
      break;
    case 0:
      c += SPOOKY_CONST;
      d += SPOOKY_CONST;
  }
  short_end (&a, &b, &c, &d);
  *hash1 = a;
  *hash2 = b;
}

void
spooky_hash128 (const void *data, size_t len, uint64_t * hash1,
                uint64_t * hash2)
{
  uint64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
  uint64_t buf[SPOOKY_NUMVARS];
  uint64_t *endp;
  union {
    const uint8_t *p8;
    uint64_t *p64;
    uintptr_t i;
  } u;
  size_t remainder;

  if (len < SPOOKY_BUFSIZE) {
    spooky_shorthash (data, len, hash1, hash2);
    return;
  }

  h0 = h3 = h6 = h9 = *hash1;
  h1 = h4 = h7 = h10 = *hash2;
  h2 = h5 = h8 = h11 = SPOOKY_CONST;

  u.p8 = (const uint8_t *) data;
  endp = u.p64 + (len / SPOOKY_BLOCKSIZE) * SPOOKY_NUMVARS;

  if (ALLOW_UNALIGNED_READS || (u.i & 0x7) == 0) {
    while (u.p64 < endp) {
      mix (u.p64, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
      u.p64 += SPOOKY_NUMVARS;
    }
  }
  else {
    while (u.p64 < endp) {
      memcpy (buf, u.p64, SPOOKY_BLOCKSIZE);
      mix (buf, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
      u.p64 += SPOOKY_NUMVARS;
    }
  }
  remainder = (len - ((const uint8_t *) endp - (const uint8_t *) data));
  memcpy (buf, endp, remainder);
  memset (((uint8_t *) buf) + remainder, 0, SPOOKY_BLOCKSIZE - remainder);
  ((uint8_t *) buf)[SPOOKY_BLOCKSIZE - 1] = remainder;
  mix (buf, &h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
  end (&h0, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11);
  *hash1 = h0;
  *hash2 = h1;
}

uint64_t
spooky_hash64 (const void *data, size_t len, uint64_t seed)
{
  uint64_t hash1 = seed;
  spooky_hash128 (data, len, &hash1, &seed);
  return hash1;
}

uint32_t
spooky_hash32 (const void *data, size_t len, uint32_t seed)
{
  uint64_t hash1 = seed, hash2 = seed;
  spooky_hash128 (data, len, &hash1, &hash2);
  return (uint32_t) hash1;
}
