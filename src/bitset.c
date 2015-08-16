#include "bitset.h"
#include "util.h"

void
bitset_init (struct bitset *h, size_t size)
{
  h->size = max (32, size);
  h->buckets = calloc (h->size / 32, sizeof (uint32_t));
}

void
bitset_free (struct bitset *h)
{
  free (h->buckets);
}

void
bitset_add (struct bitset *h, uint32_t key)
{
  key %= h->size;
  h->buckets[key / 32] |= 1 << (key % 32);
}

int
bitset_contains (struct bitset *h, uint32_t key)
{
  key %= h->size;
  return 1 & (h->buckets[key / 32] >> (key % 32));
}

void
bitset_dump (struct bitset *h, FILE * f, const char *name)
{
  size_t i;

  fprintf (f, "#include \"bitset.h\"\n");
  fprintf (f, "const struct bitset %s_bitset = {\n", name);
  fprintf (f, "  .size = %zu,\n", h->size);
  fprintf (f, "  .buckets = (uint32_t[%zu]) {\n", h->size / 32);
  for (i = 0; i < h->size / 32; i++) {
    if (i % 4 == 0)
      fprintf (f, "    ");
    fprintf (f, "0x%08x,%c", h->buckets[i], ((i % 4) < 3) ? ' ' : '\n');
  }
  fprintf (f, "  },\n");
  fprintf (f, "};\n\n");
}
