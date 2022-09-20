#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ignore_case;
int counting;
int duplicate;
FILE *fp;

void uniq()
{
  char old_line[512];
  memset(old_line, 0, sizeof(old_line));
  char new_line[512];
  memset(new_line, 0, sizeof(new_line));
  int temp_index = 0;
  int start_flag = 0;
  int count = 0;
  int c;

  while ((c = (char)fgetc(fp)) != EOF)
  {
      if (c == '\n')
      {

        if ((strcmp(old_line, new_line) != 0) && start_flag)
        {
          if (counting)
          {
            printf("\t%d ", count);
          }
          if (duplicate)
          {
            if (count > 1)
            {
              printf("%s\n", old_line);
            }
          }
          else
          {
            printf("%s\n", old_line);
          }

          count = 0;
        }
        count += 1;

        memmove(old_line, new_line, sizeof(old_line));
        memset(new_line, 0, sizeof(new_line));
        temp_index = 0;
        start_flag = 1;
      }
      else
      {

        if ((c < 91) && (c >= 65) && ignore_case)
        {
          new_line[temp_index] = c + 32;
        }
        else
        {
          new_line[temp_index] = c;
        }
        temp_index += 1;
      }
    
  }
  if ((strcmp(old_line, new_line) != 0) && start_flag)
  {
    if (counting)
    {
      printf("\t%d ", count);
    }
    if (duplicate)
    {
      if (count > 1)
      {
        printf("%s\n", old_line);
      }
    }
    else
    {
      printf("%s\n", old_line);
    }
  }
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc < 2)
  {
    while (1);
  }

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-i") == 0)
    {
      ignore_case = 1;
    }
    else if (strcmp(argv[i], "-c") == 0)
    {
      counting = 1;
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      duplicate = 1;
    }
    else
    {
      if ((fp = fopen(argv[i], "r")) == NULL)
      {
        printf("uniq: cannot open %s\n", argv[i]);
        exit(EXIT_FAILURE);
      }
    }
  }
  if (fp)
  {
    uniq();
    fclose(fp);
  }
  exit(EXIT_SUCCESS);
}
