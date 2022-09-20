#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void cat(char *file_name)
{
  FILE *fp;
  char new_line[512];

  if ((fp = fopen(file_name, "r")) == NULL)
  {
    printf("cat: cannot open %s\n", file_name);
    return;
  }

  while ((fgets(new_line, sizeof(new_line), fp)))
  {
    printf("%s", new_line);
    memset(new_line, 0, sizeof(new_line));
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc <= 1)
  {
    for (;;)
    {
      char input[512];
      scanf("%s", input);
      printf("%s\n", input);
    }
    exit(EXIT_SUCCESS);
  }

  for (i = 1; i < argc; i++)
  {
    cat(argv[i]);
  }
  exit(EXIT_SUCCESS);
}
