#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cube_pool.h"

uint64_t binary_to_uint64(char *s)
{
  uint64_t x = 0;

  for (int i = 0; s[i] != '\0'; i++)
  {
    x <<= 1;
    if (s[i] == '1')
    {
      x |= 1;
    }
  }
  return x;
}

/*
入力ファイルの行末が\r\nとならないように\0とする.
Windowsのファイルをubuntuで実行できる.
ubuntuのファイルはwindowsでは特に何もしなくても実行できる.
*/
const char *trim_newline(const char *filename)
{

  char line[256];

  FILE *fp = fopen(filename, "r");
  FILE *out_fp = fopen("trim.txt", "w");
  if (fp == NULL)
  {
    fprintf(stderr, "Cannot Open the fp file.\n");
    return NULL;
  }
  if (out_fp == NULL)
  {
    fprintf(stderr, "Cannot Open the out_fp file.\n");
    fclose(fp);
    return NULL;
  }

  while (fgets(line, sizeof(line), fp) != NULL)
  {

    line[strcspn(line, "\r\n")] = '\0';
    fprintf(out_fp, "%s\n", line);
  }

  fclose(fp);
  fflush(out_fp);
  rewind(out_fp);
  fclose(out_fp);
  return "trim.txt";
}

const char *skip_directives(const char *filename)
{
  char line[256];

  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Cannot Find Input\n");
    return NULL;
  }

  FILE *out_fp = fopen("skip.txt", "w");
  if (out_fp == NULL)
  {
    fprintf(stderr, "Cannot make skip output file\n");
    return NULL;
  }
  while (fgets(line, sizeof(line), fp) != NULL)
  {
    if (line[0] != '0' && line[0] != '1' && line[0] != '-')
    {
      continue;
    }
    fprintf(out_fp, "%s", line);
  }

  fclose(fp);

  fflush(out_fp);
  rewind(out_fp);
  fclose(out_fp);
  return "skip.txt";
}

void extract_input(const char *filename, char *output_number,
                   const char *save_filename)
{

  char line[256];
  char input[256], output[256];
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Cannot Find extra_input file.\n");
    return;
  }
  FILE *out_fp = fopen(save_filename, "w");
  if (out_fp == NULL)
  {
    fprintf(stderr, "Cannot make extra_input output file.\n");
    return;
  }

  while (fgets(line, sizeof(line), fp) != NULL)
  {
    if (output_number[0] == '\0')
    {
      if (sscanf(line, "%s", input) == 1)
      {
        fprintf(out_fp, "%s\n", input);
      }
    }
    else
    {
      if (sscanf(line, "%s %s", input, output) == 2)
      {
        if (strcmp(output, output_number) == 0)
        {
          fprintf(out_fp, "%s\n", input);
        }
      }
    }
  }
  rewind(fp);
  fclose(fp);

  fflush(out_fp);
  rewind(out_fp);
  fclose(out_fp);
  return;
}