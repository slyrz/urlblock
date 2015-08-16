#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "stringset.h"
#include "urlblock.h"

/**
 * Dummy data defined for the test.
 */
extern const struct stringset test_hosts_stringset;
extern const struct bitset test_hosts_bitset;
extern const struct stringset test_paths_stringset;
extern const struct bitset test_paths_bitset;

/**
 * The real data defined by the user.
 */
extern const struct stringset paths_stringset;
extern const struct stringset hosts_stringset;

static void
iterate (const char *bucket, void (*callback) (const char *, size_t))
{
  if (bucket == NULL)
    return;
  while (*bucket) {
    callback (bucket + 2, *bucket);
    bucket += *bucket + 2;
  }
}

static bool
block (const char *fmt, ...)
{
  va_list ap;
  char *str;
  bool res;
  struct url url = {0};

  va_start(ap, fmt);
  assert (vasprintf (&str, fmt, ap) >= 0);
  assert (url_parse (&url, str) == 0);
  free (str);
  va_end(ap);

  return url_block (&url);
}

static void
test_path (const char *needle, size_t len)
{
  if (memchr (needle, ' ', len) || memchr (needle, '%', len))
    return;
  assert (block ("%.*s", len, needle));
  assert (block ("http://example.com/some/path%.*s/and/some/more", len, needle));
  assert (block ("http://example.com/some/path?and=query&%.*s", len, needle));
}

static void
test_host (const char *needle, size_t len)
{
  assert (block ("http://%.*s", len, needle));
  assert (block ("http://subdomain.%.*s", len, needle));
  assert (block ("http://another.subdomain.%.*s", len, needle));

  assert (!block ("http://%.*s.xyz", len, needle));
  assert (!block ("http://subdomain.%.*s.xyz", len, needle));
  assert (!block ("http://subdomain_%.*s.xyz", len, needle));
  assert (!block ("http://another.subdomain.%.*s.xyz", len, needle));
}

int
main (void)
{
  size_t i;

  for (i = 0; i < paths_stringset.size; i++)
    iterate (paths_stringset.buckets[i], test_path);
  for (i = 0; i < hosts_stringset.size; i++)
    iterate (hosts_stringset.buckets[i], test_host);

  /**
   * Now mock hosts and paths with the test data.
   */
  paths = (struct lookup){
    .bitset = &test_paths_bitset, .stringset = &test_paths_stringset,
  };

  hosts = (struct lookup){
    .bitset = &test_hosts_bitset, .stringset = &test_hosts_stringset,
  };

  assert (block ("http://foo.com"));
  assert (block ("http://foo.com/"));

  assert (block ("http://www.foo.com"));
  assert (block ("http://foo.com/some/path"));
  assert (block ("http://subdomain.foo.com"));
  assert (block ("http://subdomain.foo.com/some/path"));

  assert (!block ("http://foofoo.com"));
  assert (!block ("http://subdomain.foofoo.com/"));

  assert (!block ("http://foo.com.xyz"));
  assert (!block ("http://some-domain.com/foo.com"));

  assert (block ("http://bar.baz.com"));
  assert (block ("http://subdomain.bar.baz.com"));
  assert (!block ("http://bar.baz.com.cn"));

  assert (block ("http://some-domain.com/test"));
  assert (block ("http://some-domain.com/some/path?test"));
  assert (block ("http://some-domain.com/some/path_test"));
  assert (!block ("http://test.com/some/path"));
  assert (!block ("http://some-domain.com/#path"));

  assert (block ("http://some-domain.com/test/a/b"));
  assert (block ("http://some-domain.com/a/test/c"));
  assert (block ("http://some-domain.com/a/b/test"));
  assert (block ("http://some-domain.com/a/b/test/"));
  return 0;
}
