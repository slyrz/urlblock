#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "bitset.h"
#include "buzhash.h"
#include "hash.h"
#include "stringset.h"
#include "tld.h"
#include "urlblock.h"
#include "util.h"

void
url_init (struct url *url)
{
  memset (url, 0, sizeof (struct url));
}

void
url_free (struct url *url)
{
  freenull (url->buffer.data);
  freenull (url->buffer.temp);
  memset (url, 0, sizeof (struct url));
}

int
url_grow (struct url *url, size_t cap)
{
  /**
   * Make sure there's room for a trailing zero.
   */
  cap++;

  if (cap > url->buffer.cap || url->buffer.data == NULL || url->buffer.temp == NULL) {
    while (cap > url->buffer.cap)
      url->buffer.cap = max (url->buffer.cap, 2048) << 1;

    freenull (url->buffer.data);
    freenull (url->buffer.temp);

    url->buffer.data = calloc (url->buffer.cap, sizeof (char));
    url->buffer.temp = calloc (url->buffer.cap, sizeof (char));

    if (url->buffer.data == NULL || url->buffer.temp == NULL) {
      freenull (url->buffer.data);
      freenull (url->buffer.temp);
      return -1;
    }
  }
  return 0;
}

/**
 * Created during build. Can be found in data/hosts.c and data/paths.c.
 */
extern const struct stringset hosts_stringset;
extern const struct bitset hosts_bitset;
extern const struct stringset paths_stringset;
extern const struct bitset paths_bitset;
extern const struct stringset tlds_stringset;
extern const struct bitset tlds_bitset;


struct lookup hosts = {
  .bitset = &hosts_bitset,
  .stringset = &hosts_stringset,
};

struct lookup paths = {
  .bitset = &paths_bitset,
  .stringset = &paths_stringset,
};

struct lookup tlds = {
  .bitset = &tlds_bitset,
  .stringset = &tlds_stringset,
};

static int
unhex (int c)
{
  if (('0' <= c) && (c <= '9'))
    return c - '0';
  if (('a' <= c) && (c <= 'f'))
    return c - 'a' + 10;
  if (('A' <= c) && (c <= 'F'))
    return c - 'A' + 10;
  return 0;
}

/**
 * unescape transform percent encoded characters into their ASCII
 * representation.
 *
 * Since unescaping does - if anything - reduce string size, this function
 * works inplace.
 */
static void
unescape (char *str, bool query)
{
  char *dst = str;

  while (*str) {
    if (*str == '%') {
      if (isxdigit (str[1]) && isxdigit (str[2])) {
        str[2] = unhex (str[1]) << 4 | unhex (str[2]);
        str += 2;
      }
    }
    else {
      if (query && (*str == '+'))
        *str = ' ';
    }
    *dst++ = *str++;
  }
  if (dst != str)
    *dst = '\0';
}

/**
 * cutoff finds the first occurence of character c, replaces the character
 * with a null byte and returns the remaining part of the string.
 */
static char *
cutoff (char *str, int c)
{
  str = strchr (str, c);
  if (str)
    *str++ = '\0';
  return str;
}

static inline bool
isschemechar (int c)
{
  return (isalnum (c) || c == '+' || c == '-' || c == '.');
}

static void
copystr (char *dst, char *src, size_t n)
{
  bcopy (src, dst, n);
  dst[n] = '\0';
}

static char *
parse_query_and_fragment (struct url *url, char *restrict str)
{
  url->parts.fragment = cutoff (str, '#');
  url->parts.rawquery = cutoff (str, '?');
  if (url->parts.fragment)
    unescape (url->parts.fragment, false);
  return str;
}

static char *
parse_scheme (struct url *url, char *restrict str)
{
  char *pos = str;
  char *end;

  if (!isalpha (*str))
    return str;
  end = strchrnul (pos, ':');
  if (*end == '\0')
    return str;
  *end = '\0';
  while (*pos) {
    if (!isschemechar (*pos))
      return "";
    *pos = tolower (*pos);
    pos++;
  }
  url->parts.scheme = str;
  return end + 1;
}

static char *
parse_server (struct url *url, char *restrict str)
{
  bool escaped = false;
  char *pos;
  char *end;
  char *ret;

  if ((!url->parts.scheme) || (strncmp (str, "//", 2) != 0))
    return str;

  pos = str;

  /**
   * Search for the beginning of the path component.
   */
  end = strchrnul (str + 2, '/');
  ret = end;

  /**
   * If a path component follows the authority, we need to create extra space
   * for a null-terminator. Shift the host 2 spaces to the left and
   * override the starting "//".
   */
  if (*end)
    copystr (pos, pos + 2, end - pos - 2);
  else
    pos += 2;

  /**
   * Skip user credentials.
   */
  end = pos;
  while (*end) {
    /**
     * The at sign means we were parsing the userinfo, not the host part.
     */
    if (*end == '@') {
      pos = end + 1;
      escaped = false;
    }
    escaped |= (*end == '%');
    *end = tolower (*end);
    end++;
  }

  /**
   * Trim port number
   */
  if (end > pos) {
    end--;
    while (isdigit (*end))
      end--;
    if (*end == ':')
      *end = '\0';
  }

  url->parts.host = pos;
  if (escaped)
    unescape (url->parts.host, false);
  return ret;
}

static char *
parse_path (struct url *url, char *restrict str)
{
  if (*str) {
    url->parts.path = str;
    unescape (url->parts.path, false);
  }
  return "";
}

int
url_parse (struct url *url, const char *restrict str)
{
  char *buffer;

  if (url_grow (url, strlen (str)) != 0)
    goto error;

  memset (&url->parts, 0, sizeof (url->parts));
  buffer = strcpy (url->buffer.data, str);

  /**
   * How the parsing works. The inserted vertical bars indicate the
   * position of null-terminators.
   *
   *  1) Cut off the fragment. The fragment starts at the first occurrence
   *     of a #.
   *
   *        http://example.org/path/?query=xyz|fragment
   *                                           \______/  = parts.fragment
   *
   *  2) Cut off the query. The query starts at the first occurrence
   *     of a ?.
   *
   *        http://example.org/path/|query=xyz|fragment
   *                                 \_______/           = parts.rawquery
   *
   *  3) Cut off the scheme.
   *
   *        http|//example.org/path/|query=xyz|fragment
   *        \__/                                         = parts.scheme
   *
   *  4) Parse host. Since a null-terminator can't be placed at the host's
   *     end because it's the start the path, shift the host to the left and
   *     overwrite "//".
   *
   *        http|example.org||/path/|query=xyz|fragment
   *             \_________/                             = parts.host
   *
   *  5) Parse path.
   *
   *        http|example.org||/path/|query=xyz|fragment
   *                          \____/                     = parts.path
   */
  buffer = parse_query_and_fragment (url, buffer);
  buffer = parse_scheme (url, buffer);
  buffer = parse_server (url, buffer);
  buffer = parse_path (url, buffer);

  if (!url->parts.host && !url->parts.path)
    goto error;
  return 0;
error:
  memset (&url->parts, 0, sizeof (url->parts));
  return -1;
}

static inline bool
contains (uint32_t k, const char *v, unsigned char *m, const struct lookup *l)
{
  if (bitset_contains ((struct bitset *) l->bitset, k))
    return stringset_contains ((struct stringset *) l->stringset, k, v, m);
  return false;
}

static bool
block_host (struct url *url)
{
  const char *host = url->parts.host;
  char *next;

  if (host == NULL)
    return 0;

  /**
   * Returns true if a host name or any of its higher level hosts is blocked.
   *
   *   'foo.bar.baz.qux.com'
   *    \_________________/
   *        \_____________/
   *            \_________/
   *                \_____/
   */
  while ((next = strchr (host, '.'))) {
    if (contains (hash32 (host, strlen (host)), host, NULL, &hosts))
      return 1;
    host = next + 1;
  }
  return 0;
}

static bool
block_path_and_query (struct url *url)
{
  const char *path = url->parts.path ? url->parts.path : "";
  const char *query = url->parts.rawquery ? url->parts.rawquery : "";
  uint32_t k;
  size_t i;
  size_t n;

  n = snprintf (url->buffer.temp, url->buffer.cap, "%s?%s", path, query);
  if (n < buzhash_width)
    return 0;
  k = buzhash (url->buffer.temp);
  for (i = 0; i <= n - buzhash_width; i++) {
    if (contains (k, url->buffer.temp + i, NULL, &paths))
      return 1;
    k = buzhash_update (url->buffer.temp + i, k);
  }
  return 0;
}

int
url_block (struct url *url)
{
  return block_host (url) || block_path_and_query (url);
}

const char *
url_public_suffix (struct url *url)
{
  const char *host = url->parts.host;
  const char *pos[3] = {
    NULL,
    NULL,
    host
  };

  if ((host == NULL) || (*host == '.'))
    return NULL;

  /**
   * Move from dot to dot, keep pointers as follows:
   *
   *   'foo.bar.baz.qux.com'
   *        \_____________/  = pos[0]
   *            \_________/  = pos[1]
   *                \_____/  = pos[2]
   */
  while (*pos[2]) {
    unsigned char mask;
    if (contains (hash32 (pos[2], strlen (pos[2])), pos[2], &mask, &tlds)) {
      switch (mask) {
        case EXCEPTION:
          return pos[2];
        case WILDCARD:
          return pos[0];
        default:
          return pos[1];
      }
    }
    pos[0] = pos[1];
    pos[1] = pos[2];
    pos[2] = strchr (pos[2], '.');
    if (pos[2] == NULL)
      break;
    pos[2]++;                   // skip '.'
    if (*pos[2] == '.')
      return NULL;
  }
  return pos[0];
}
