#ifndef URLBLOCK_H
#define URLBLOCK_H

#include <stddef.h>
#include <stdint.h>

struct lookup {
  const struct bitset *bitset;
  const struct stringset *stringset;
};

extern struct lookup hosts;
extern struct lookup paths;
extern struct lookup tlds;

struct url {
  struct parts {
    char *scheme;
    char *host;
    char *path;
    char *rawquery;
    char *fragment;
  } parts;
  /**
   * Private fields.
   */
  struct {
    size_t cap;
    char *data;
    char *temp;
  } buffer;
};

void url_init (struct url *url);
void url_free (struct url *url);

int url_parse (struct url *url, const char *str);
int url_block (struct url *url);

const char *url_public_suffix (struct url *url);

#endif
