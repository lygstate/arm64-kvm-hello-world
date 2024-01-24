#define _GNU_SOURCE

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/times.h>
#include <sys/stat.h>

/* errno definition */
#undef errno
extern int errno;

char *__env[1] = {0};
char **environ = __env;

int _close(int file)
{
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}

void _exit(int)
{
  for (;;)
  {
  }
}

int _fork(void)
{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid()
{
  return 1;
}

int _kill(int pid, int sig)
{
  errno = EINVAL;
  return (-1);
}

int isatty(int fildes)
{
  return 1;
}

int _isatty(int fildes)
{
  return 1;
}

int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _open(const char *name, int flags, int mode)
{
  return -1;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

int _stat(char *file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _times(struct tms *buf)
{
  return -1;
}

int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

volatile uint32_t *const UART0DR = (uint32_t *)0x09000000;

void print_uart0(const char *s, int len)
{
  /* Loop until end of string */
  for (int i = 0; i < len; i += 1)
  {
    *UART0DR = (uint32_t)(s[i]); /* Transmit char */
  }
}

int _write(int file, char *ptr, int len)
{
  /* 1 is stdout */
  if (file == 1 || file == 2)
  {
    print_uart0(ptr, len);
  }
  return len;
}

extern char heap_start[];
extern char heap_stop[];
static char *heap_ptr = heap_start;

char *
_sbrk(int nbytes)
{
  char *base = heap_ptr;
  heap_ptr += nbytes;
  if (heap_ptr > heap_stop)
  {
    errno = ENOMEM;
    return ((char *)-1);
  }

  return base;
}