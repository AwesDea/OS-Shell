#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int reverse;
int new_file;
int numerically;
char lines[100][512];
FILE *fp_dest;
FILE *fp_src;

void sort(int line_count)
{
  char swap[512];

  for (int i = 0; i < line_count - 1; i++)
  {
    for (int j = 0; j < line_count - i - 1; j++)
    {
      if (!numerically)
      {
        memset(swap, 0, sizeof(swap));
        for (int count = 0; count < sizeof(lines[j]); count++)
        {
          char first = lines[j][count];
          char second = lines[j + 1][count];

          if ((first < 91) && (first >= 65))
          {
            first += 32;
          }
          if ((second < 91) && (second >= 65))
          {
            second += 32;
          }

          if (first > second)
          {
            memset(swap, 0, sizeof(swap));
            strcpy(swap, lines[j]);
            memset(lines[j], 0, sizeof(lines[j]));
            strcpy(lines[j], lines[j + 1]);
            memset(lines[j + 1], 0, sizeof(lines[j + 1]));
            strcpy(lines[j + 1], swap);
            break;
          }
          else if (first == second)
          {
            if (lines[j][count] < lines[j + 1][count])
            {
              memset(swap, 0, sizeof(swap));
              strcpy(swap, lines[j]);
              memset(lines[j], 0, sizeof(lines[j]));
              strcpy(lines[j], lines[j + 1]);
              memset(lines[j + 1], 0, sizeof(lines[j + 1]));
              strcpy(lines[j + 1], swap);
              break;
            }
          }
          else
          {
            break;
          }
          if (lines[j][count] == '\0' && lines[j + 1][count] == '\0')
            break;
        }
      }
      else
      {
        if (atoi(lines[j]) > atoi(lines[j + 1]))
        {
          memset(swap, 0, sizeof(swap));
          strcpy(swap, lines[j]);
          memset(lines[j], 0, sizeof(lines[j]));
          strcpy(lines[j], lines[j + 1]);
          memset(lines[j + 1], 0, sizeof(lines[j + 1]));
          strcpy(lines[j + 1], swap);
        }
      }
    }
  }
  if (reverse)
  {
    for (int i = line_count - 1; i > -1; i--)
    {
      if (new_file)
      {
        int len = 512;
        for (int j = 0; j < sizeof(lines[i]); j++)
        {
          if (lines[i][j] == '\0')
          {
            len = j;
            break;
          }
        }
        if (fputs(lines[i], fp_dest) == EOF)
        {
          printf("sort: cannot write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        printf("%s", lines[i]);
      }
    }
  }
  else
  {
    for (int i = 0; i < line_count; i++)
    {
      if (new_file)
      {
        int len = 512;
        for (int j = 0; j < sizeof(lines[i]); j++)
        {
          if (lines[i][j] == '\0')
          {
            len = j;
            break;
          }
        }
        if (fputs(lines[i], fp_dest) == EOF)
        {
          printf("sort: cannot write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        printf("%s", lines[i]);
      }
    }
  }
  exit(EXIT_SUCCESS);
}

void readFile()
{
  int line_count = 0;
  memset(lines, 0, sizeof(lines));
  char new_line[512];
  memset(new_line, 0, sizeof(new_line));

  while ((fgets(new_line, sizeof(new_line), fp_src)))
  {
    strcpy(lines[line_count], new_line);
    memset(new_line, 0, sizeof(new_line));
    line_count += 1;
  }
  sort(line_count);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc < 2)
  {
    while (1)
      ;
  }

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-r") == 0)
    {
      reverse = 1;
    }
    else if (strcmp(argv[i], "-o") == 0)
    {
      new_file = 1;
    }
    else if (strcmp(argv[i], "-n") == 0)
    {
      numerically = 1;
    }
    else if (fp_src)
    {
      if (new_file)
      {
        if (((fp_dest = fopen(argv[i], "w")) == NULL))
        {
          printf("sort: cannot open %s\n", argv[i]);
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        printf("sort: cannot create %s. add -o!\n", argv[i]);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      if ((fp_src = fopen(argv[i], "r")) == NULL)
      {
        printf("sort: cannot open %s\n", argv[i]);
        exit(EXIT_FAILURE);
      }
    }
  }
  if (fp_src)
  {
    readFile();
    fclose(fp_src);
    if (fp_dest)
    {
      fclose(fp_dest);
    }
  }

  exit(EXIT_SUCCESS);
}
