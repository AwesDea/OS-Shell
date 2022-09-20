#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("mkdir: missing operand\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; i++)
  {
    if (mkdir(argv[i], 0777) < 0)
    {
      printf("mkdir: %s failed to create\n", argv[i]);
    }
  }

  exit(EXIT_SUCCESS);
}
