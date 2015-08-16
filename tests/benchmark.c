#include <stdio.h>
#include <time.h>
#include "urlblock.h"

#define N 500000

/**
 * Repeate function N times.
 */
#define repeat(f) \
  do { \
    int i; \
    for (i = 0; i < N; i++) { \
      f; \
    } \
  } while (0)

/**
 * Execute function 'do_##func' and measure the execution time.
 */
#define benchmark(func,...) \
  do { \
    clock_t t; \
    double s; \
    t = clock (); \
    do_ ## func (__VA_ARGS__); \
    s = (double) (clock () - t) / CLOCKS_PER_SEC; \
    printf ("%-20s\t%10.3fs (%.0f/s)\n", #func, s, N / s); \
  } while (0)

#pragma optimization_level 0

static void
do_url_parse (const char *rawurl)
{
  struct url url = {0};
  repeat (url_parse (&url, rawurl));
}

static void
do_url_block (const char *rawurl)
{
  struct url url = {0};
  url_parse (&url, rawurl);
  repeat (url_block (&url));
}

static void
do_url_public_suffix (const char *rawurl)
{
  struct url url = {0};
  url_parse (&url, rawurl);
  repeat (url_public_suffix (&url));
}

int
main (void)
{
  benchmark (url_parse, "http://www.example.com/");
  benchmark (url_block, "http://www.example.com/");
  benchmark (url_public_suffix, "http://www.example.com/");
}
