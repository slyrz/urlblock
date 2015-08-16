#include <stdio.h>

#include "urlblock.h"
#include "file.h"

static struct url url = { 0 };

static void
check_parse (const char *rawurl)
{
  url_parse (&url, rawurl);
}

int
main (int argc, char **argv)
{
  struct file f;
  char *line;
  int i;
  int n;

  n = 0;
  for (i = 1; i < argc; i++) {
    file_open (&f, argv[i]);
    while ((line = file_readline (&f))) {
      check_parse (line);
      n++;
    }
    file_close (&f);
  }
  url_free (&url);

  printf ("Tested %d urls.\n", n);
}
