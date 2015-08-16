#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "urlblock.h"

struct test_case {
  const char *inp;
  const char *scheme;
  const char *host;
  const char *path;
  const char *rawquery;
  const char *fragment;
  int fail;
};

static struct test_case cases[] = {
  {"http://www.example.com", "http", "www.example.com", NULL, NULL, NULL, 0},
  {"http://www.example.com/", "http", "www.example.com", "/", NULL, NULL, 0},
  {"HTTP://www.example.com/", "http", "www.example.com", "/", NULL, NULL, 0},
  {"http://WWW.EXAMPLE.COM/", "http", "www.example.com", "/", NULL, NULL, 0},
  {"http://www.example.com/file%20one%26two", "http", "www.example.com", "/file one&two", NULL, NULL, 0},
  {"ftp://webmaster@www.example.com/", "ftp", "www.example.com", "/", NULL, NULL, 0},
  {"ftp://john%20doe@www.example.com/", "ftp", "www.example.com", "/", NULL, NULL, 0},
  {"http://www.example.com/?q=some+query", "http", "www.example.com", "/", "q=some+query", NULL, 0},
  {"http://www.example.com/?q=some%20query", "http", "www.example.com", "/", "q=some%20query", NULL, 0},
  {"http://%2f%2fwww.example.com/?q=some+query", "http", "//www.example.com", "/", "q=some+query", NULL, 0},
  {"mailto:/webmaster@golang.org", "mailto", NULL, "/webmaster@golang.org", NULL, NULL, 0},
  {"mailto:webmaster@golang.org", "mailto", NULL, "webmaster@golang.org", NULL, NULL, 0},
  {"/foo?query=http://bad", NULL, NULL, "/foo", "query=http://bad", NULL, 0},
  {"//foo", NULL, NULL, "//foo", NULL, NULL, 0},
  {"///threeslashes", NULL, NULL, "///threeslashes", NULL, NULL, 0},
  {"http://user:password@example.com", "http", "example.com", NULL, NULL, NULL, 0},
  {"http://j@ne:password@example.com", "http", "example.com", NULL, NULL, NULL, 0},
  {"http://jane:p@ssword@example.com", "http", "example.com", NULL, NULL, NULL, 0},
  {"http://j@ne:password@example.com/p@th?q=@go", "http", "example.com", "/p@th", "q=@go", NULL, 0},
  {"http://www.example.com/?q=some+query#foo", "http", "www.example.com", "/", "q=some+query", "foo", 0},
  {"http://www.example.com/?q=some+query#foo%26bar", "http", "www.example.com", "/", "q=some+query", "foo&bar", 0},
  {"file:///home/user/path", "file", NULL, "/home/user/path", NULL, NULL, 0},
  {"file:///C:/FooBar/Baz.txt", "file", NULL, "/C:/FooBar/Baz.txt", NULL, NULL, 0},
  {"a/b/c", NULL, NULL, "a/b/c", NULL, NULL, 0},
  {"http://%3Fam:pa%3Fsword@example.com", "http", "example.com", NULL, NULL, NULL, 0},
  {"http://192.168.0.1/", "http", "192.168.0.1", "/", NULL, NULL, 0},
  {"http://192.168.0.1:8080/", "http", "192.168.0.1", "/", NULL, NULL, 0},
  {"http://[fe80::1]/", "http", "[fe80::1]", "/", NULL, NULL, 0},
  {"http://[fe80::1]:8080/", "http", "[fe80::1]", "/", NULL, NULL, 0},
  {"http://[fe80::1%25en0]/", "http", "[fe80::1%en0]", "/", NULL, NULL, 0},
  {"http://[fe80::1%25en0]:8080/", "http", "[fe80::1%en0]", "/", NULL, NULL, 0},
  {"http://[fe80::1%25%65%6e%301-._~]/", "http", "[fe80::1%en01-._~]", "/", NULL, NULL, 0},
  {"http://[fe80::1%25%65%6e%301-._~]:8080/", "http", "[fe80::1%en01-._~]", "/", NULL, NULL, 0},
  {"http://user:pass@foo:21/bar;par?b#c", "http", "foo", "/bar;par", "b", "c", 0},
  {"http://foo.com", "http", "foo.com", NULL, NULL, NULL, 0},
  {"http//:www.example.com/?q=some+query", NULL, NULL, NULL, NULL, NULL, 1},
  {"http://www.example.คอม", "http", "www.example.คอม", NULL, NULL, NULL, 0},
  {"http://www.example.在线", "http", "www.example.在线", NULL, NULL, NULL, 0},
  {"http://旚旙旅旗旌晐晱柯.新闻", "http", "旚旙旅旗旌晐晱柯.新闻", NULL, NULL, NULL, 0},
  {"http://benib.bbvujfnmr.od.商城/旅旗旌", "http", "benib.bbvujfnmr.od.商城", "/旅旗旌", NULL, NULL, 0},
  {"http://%E6%97%9A%E6%97%99%E6%97%85%E6%97%97%E6%97%8C%E6%99%90%E6%99%B1%E6%9F%AF.%E6%96%B0%E9%97%BB", "http", "旚旙旅旗旌晐晱柯.新闻", NULL, NULL, NULL, 0},
};

static void
compare (const char *out, const char *exp)
{
  if (out == NULL)
    out = "";
  if (exp == NULL)
    exp = "";
  assert (strcmp (out, exp) == 0);
}

int
main (void)
{
  struct url url = {0};
  size_t n = sizeof (cases) / sizeof (cases[0]);
  size_t i;

  for (i = 0; i < n; i++) {
    puts (cases[i].inp);
    assert ((url_parse (&url, cases[i].inp) != 0) == cases[i].fail);
    if (cases[i].fail)
      continue;
    compare (url.parts.scheme, cases[i].scheme);
    compare (url.parts.host, cases[i].host);
    compare (url.parts.path, cases[i].path);
    compare (url.parts.rawquery, cases[i].rawquery);
    compare (url.parts.fragment, cases[i].fragment);
  }
  return 0;
}
