#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <limits.h>

#include "stringset.h"
#include "util.h"

/**
 *  Memory layout:
 *
 *   unsigned char length;
 *   unsigned char bitmask;
 *   char data[];
 */
static void
string_append (char **dst, const char *value, unsigned char bitmask)
{
  char *old = "";
  int ret;

  /**
   * Length will be stored in a single char, so strings cannot be longer
   * than UCHAR_MAX.
   */
  if (strlen (value) > UCHAR_MAX)
    err (EXIT_FAILURE, "value exceeds %d chars", UCHAR_MAX);

  /**
   * At least 1 bit needs to be unset, otherwise this turns into '\0' and
   * messes up the storage.
   */
  if (bitmask & 0x80)
    err (EXIT_FAILURE, "bitmask exceeds 7 bit");

  if (*dst)
    old = *dst;
  ret = asprintf (dst, "%s%c%c%s", old, (int) strlen (value), ~bitmask, value);
  if (ret < 0)
    err (EXIT_FAILURE, "asprintf");
  if (*old)
    free (old);
}

static int
bucket_contains (const char *bucket, const char *value, unsigned char *bitmask)
{
  if (bucket == NULL)
    return 0;
  while (*bucket) {
    if (memcmp (bucket + 2, value, (size_t) bucket[0]) == 0) {
      if (bitmask)
        *bitmask = ~bucket[1] & 0xff;
      return 1;
    }
    bucket += *bucket + 2;
  }
  return 0;
}

void
stringset_init (struct stringset *h, size_t size)
{
  h->size = max (1, size);
  h->buckets = calloc (h->size, sizeof (char *));
}

void
stringset_free (struct stringset *h)
{
  size_t i;

  for (i = 0; i < h->size; i++)
    free (h->buckets[i]);
  free (h->buckets);
}

void
stringset_add (struct stringset *h, uint32_t key, const char *value, unsigned char bitmask)
{
  size_t pos = key % h->size;

  if (bucket_contains (h->buckets[pos], value, NULL))
    return;
  string_append (h->buckets + pos, value, bitmask);
}

int
stringset_contains (struct stringset *h, uint32_t key, const char *value, unsigned char *bitmask)
{
  return bucket_contains (h->buckets[key % h->size], value, bitmask);
}

void
stringset_dump (struct stringset *h, FILE * f, const char *name)
{
  size_t i;

  fprintf (f, "#include \"stringset.h\"\n");
  fprintf (f, "const struct stringset %s_stringset = {\n", name);
  fprintf (f, "  .size = %zu,\n", h->size);
  fprintf (f, "  .buckets = (char *[%zu]) {\n", h->size);
  for (i = 0; i < h->size; i++) {
    char *b = h->buckets[i];
    if (b == NULL)
      fprintf (f, "    NULL,\n");
    else
      for (; *b; b += 2 + *b)
        fprintf (f, "    \"\\x%02x\\x%02x\" \"%.*s\"%s\n", b[0] & 0xff, b[1] & 0xff, b[0], b + 2, (b[2 + *b]) ? "" : ",");
  }
  fprintf (f, "  },\n");
  fprintf (f, "};\n\n");
}
