#include "file.h"

#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void
file_open (struct file *f, const char *path)
{
  struct stat s;
  size_t i;

  memset (f, 0, sizeof (struct file));

  f->fd = open (path, O_RDONLY);
  if (f->fd < 0)
    err (EXIT_FAILURE, "open");
  if (fstat (f->fd, &s) != 0)
    err (EXIT_FAILURE, "fstat");
  f->size = s.st_size;
  if (f->size) {
    f->data = mmap (0, f->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, f->fd, 0);
    if (f->data == MAP_FAILED)
      err (EXIT_FAILURE, "mmap");
    for (i = 0; i < f->size; i++)
      f->lines += (f->data[i] == '\n');
  }
}

void
file_close (struct file *f)
{
  if (f->data)
    munmap (f->data, f->size);
  close (f->fd);
}

char *
file_readline (struct file *f)
{
  size_t i;
  size_t j;
  size_t k;

  if (f->pos >= f->size)
    return NULL;
  /* Skip leading whitespace */
  for (i = f->pos; i < f->size; i++)
    if (f->data[i] > ' ')
      break;
  /* Find end of line */
  for (j = i + 1; j < f->size; j++)
    if ((f->data[j] == '\n') || (f->data[j] == '\r'))
      break;
  if (j >= f->size)
    return NULL;
  /* Trim trailing spaces */
  for (k = j; k > i; k--)
    if (f->data[k - 1] > ' ')
      break;
  f->data[k] = '\0';
  f->pos = j + 1;
  return (char *) f->data + i;
}
