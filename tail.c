#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char lines[100][512];

void tail(char *file_name, int lines_count, int multi_file)
{
  FILE *fp;
  char new_line[512];
  memset(new_line, 0, sizeof(new_line));
  int counter = 0;

  if ((fp = fopen(file_name, "r")) == NULL)
  {
    printf("tail: cannot open '%s'\n", file_name);
    return;
  }
  if (multi_file)
  {
    printf("==> %s <==\n", file_name);
  }
  while ((fgets(new_line, sizeof(new_line), fp)))
  {
    strcpy(lines[counter], new_line);
    memset(new_line, 0, sizeof(new_line));
    counter += 1;
  }

  int start = 0;

  if (counter > lines_count)
  {
    start = counter - lines_count;
  }

  for (int i = start; i < counter; i++)
  {
    printf("%s", lines[i]);
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  int i;
  int lines_count = 10;
  int arg_counter = 1;
  int multi_file = 0;

  if (argc <= 1)
  {
    for (;;)
      ;
    exit(EXIT_SUCCESS);
  }

  if (strcmp(argv[1], "-n") == 0)
  {
    for (int i = 0; i < sizeof(&argv[2]); i++)
    {
      if (argv[2][i] == '\0')
      {
        break;
      }

      if (!isdigit((argv[2][i])))
      {
        printf("head: invalid number of lines: '%s'\n", argv[2]);
        exit(EXIT_FAILURE);
      }
    }
    lines_count = atoi(argv[2]);
    arg_counter = 3;
  }
  if (argc - arg_counter > 1)
    multi_file = 1;

  if (argc == arg_counter)
  {
    for (;;)
      ;
    exit(EXIT_SUCCESS);
  }

  int nflag = 0;
  for (i = arg_counter; i < argc; i++)
  {
    if (nflag)
    {
      printf("\n");
    }
    tail(argv[i], lines_count, multi_file);
    nflag = 1;
  }
  exit(EXIT_SUCCESS);
}
