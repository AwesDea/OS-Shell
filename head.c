#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void head(char *file_name, int lines_count)
{
  int n;
  char new_line[512];
  int counter = 0;
  FILE *fp = fopen(file_name, "r");

  while ((fgets(new_line, sizeof(new_line), fp)))
  {
    if (counter == lines_count)
      break;

    printf("%s", new_line);
    memset(new_line, 0, sizeof(new_line));
    counter += 1;
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{
  int i;
  FILE *fp;
  int lines_count = 10;
  int arg_counter = 1;
  int multi_file = 0;

  if (argc <= 1)
  {
    for (int i = 0; i < lines_count; i++)
    {
      char input[512];
      scanf("%s", input);
      printf("%s\n", input);
    }
    exit(EXIT_SUCCESS);
  }

  if (strcmp(argv[1], "-n") == 0)
  {
    if (argc < 3)
    {
      printf("head: option requires an argument -- 'n'\n");
      exit(EXIT_FAILURE);
    }

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

  if (argc - arg_counter < 1)
  {
    for (int i = 0; i < lines_count; i++)
    {
      char input[512];
      scanf("%s", input);
      printf("%s\n", input);
    }
    exit(EXIT_SUCCESS);
  }

  for (i = arg_counter; i < argc; i++)
  {
    if ((fp = fopen(argv[i], "r")) == NULL)
    {
      printf("head: cannot open %s\n", argv[i]);
      exit(EXIT_FAILURE);
    }
    if (fp)
    {
      if (multi_file)
      {
        printf("==> %s <==\n", argv[i]);
      }
      fclose(fp);
      head(argv[i], lines_count);

      if (i < argc - 1)
      {
        printf("\n");
      }
    }
  }
  exit(EXIT_SUCCESS);
}
