#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

#include "bitset.h"
#include "buzhash.h"
#include "file.h"
#include "stringset.h"
#include "tld.h"
#include "util.h"

#include "hash.h"

static char *typestr;
static char *namestr;

static enum { UNDEFINED = 0, HOSTS, PATHS, TLDS } type = UNDEFINED;
static double load_stringset = 4.0;
static double load_bitset = 0.25;

static void
usage (void)
{
  fprintf (stderr, "Usage: ./generate [OPTION]... FILE\n\n"
                   "Option:\n"
                   "  -t <string>       define type, either 'hosts' or 'paths'\n"
                   "  -n <string>       define the variable name\n"
                   "  -b <float>        set load factor of bloom filter\n"
                   "  -h <float>        set load factor of hash table\n");
  exit (EXIT_FAILURE);
}

static void
lower (char *str)
{
  for (; *str; str++)
    *str = tolower (*str);
}

int
main (int argc, char **argv)
{
  struct stringset s;
  struct bitset b;
  struct file f;

  unsigned char mask = 0;
  char *line;
  int c;

  uint32_t hash;

  while ((c = getopt (argc, argv, "t:n:b:h:")) != -1)
    switch (c) {
      case 't':
        typestr = optarg;
        if (strcmp (typestr, "hosts") == 0)
          type = HOSTS;
        if (strcmp (typestr, "paths") == 0)
          type = PATHS;
        if (strcmp (typestr, "tlds") == 0)
          type = TLDS;
        break;
      case 'n':
        namestr = optarg;
        break;
      case 'b':
        load_bitset = atof (optarg);
        break;
      case 'h':
        load_stringset = atof (optarg);
        break;
      case '?':
        usage ();
    }

  if (type == UNDEFINED)
    errx (EXIT_FAILURE, "unkown type '%s'", typestr);

  if (namestr == NULL)
    namestr = typestr;

  if (optind >= argc)
    usage ();

  file_open (&f, argv[optind]);
  bitset_init (&b, optsize (f.lines, load_bitset));
  stringset_init (&s, optsize (f.lines, load_stringset));

  for (;;) {
    line = file_readline (&f);
    if (line == NULL)
      break;
    if (type == HOSTS)
      lower (line);
    if (type == TLDS) {
      if (strncmp (line, "//", 2) == 0)
        continue;
      mask = 0;
      if (*line == '!')
        mask = EXCEPTION;
      if (*line == '*')
        mask = WILDCARD;
      while (*line == '.' || *line == '*' || *line == '!')
        line++;
    }
    if (type == PATHS)
      hash = buzhash (line);
    else
      hash = hash32 (line, strlen (line));
    bitset_add (&b, hash);
    stringset_add (&s, hash, line, mask);
  }

  stringset_dump (&s, stdout, namestr, type != PATHS);
  bitset_dump (&b, stdout, namestr);

  stringset_free (&s);
  bitset_free (&b);
  file_close (&f);
  return 0;
}
