#ifndef URLBLOCK_FILE_H
#define URLBLOCK_FILE_H

#include <stdlib.h>

struct file {
  int fd;
  unsigned char *data;
  size_t size;
  size_t lines;
  size_t pos;
};

void file_open (struct file *, const char *);
void file_close (struct file *);
char *file_readline (struct file *);

#endif
