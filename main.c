/*
  Copyright (c) 2019-2021 John Miner

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#define _POSIX_C_SOURCE 1
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define buflen 999999999 
#define contentslen 999999999

unsigned char wavBuf[buflen];
char contents[contentslen];
size_t mystrnlen(char *s, size_t maxlen);
int main(int argc, char *argv[])
{
  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int len = 0;
  unsigned int wavelenstrlen = 0;
  char wavelenstr[255];
  FILE *f;
  FILE *e;
  size_t totalwritten = 0;
  size_t numtowrite = 0;
  size_t numwritten = 0;
  size_t namelen = 0;
  int lastpercent = -1;
  unsigned char buf[256];
  contents[0] = '\0';
  if (argc != 3) {
    fprintf(stderr, "Usage: ./wavToRwops in.wav out.c\n");
    exit(1);
  }
  e = fopen(argv[1], "rb");
  if (e == NULL) {
    fprintf(stderr, "Failed to open input file\n");
    exit(1);
  }
  while(feof(e) == 0) {
    size_t numread = fread(buf, sizeof(unsigned char), 256, e);
    if (ferror(e)) {
      fprintf(stderr, "Read error: %d. Exiting.\n", errno);
      fclose(e);
      exit(1);
    }
    for (i = 0; i < numread; ++i) {
      wavBuf[j++] = buf[i];
      if ((j + 1) == buflen) {
        fprintf(stderr, "Too big.\n");
        fclose(e);
        exit(1);
      }
    }
  }
  len=j - 1;
  fclose(e);
  f = fopen(argv[2], "w");
  if (f == NULL) {
    fprintf(stderr, "Failed to open output file\n");
    exit(1);
  }
  namelen = mystrnlen(argv[1], 255);
  strncat(contents, "#define ", 8);
  strncat(contents, argv[1], namelen-4);
  strncat(contents, "wavlen ", 7);
  sprintf(wavelenstr, "%d\n", len+1);
  wavelenstrlen = mystrnlen(wavelenstr, 255);
  strncat(contents, wavelenstr, wavelenstrlen);
  strncat(contents, "Uint8 ", 6);
  strncat(contents, argv[1], namelen-4);
  strncat(contents, "wav[", 4);
  strncat(contents, argv[1], namelen-4);
  strncat(contents, "wavlen", 6);
  strncat(contents, "] = { ", 6);
  numtowrite = mystrnlen(contents, contentslen);
  totalwritten = 0;
  while(numtowrite > 0) {
    numwritten = fwrite(&(contents[totalwritten]), sizeof(char), numtowrite, f);
    if (ferror(f)) {
      fprintf(stderr, "Write error. Output file garbled. Exiting.\n");
      fclose(f);
      exit(1);
    }
    totalwritten += numwritten;
    numtowrite -= numwritten;
  }
  for (i = 0; i < len; ++i) {
    char piece[6];
    size_t piecelen = 0;
    int percent = (int) ((double) i / (double) len * 100.0);
    contents[0] = '\0';
    if (percent != lastpercent)
      fprintf(stdout, "%d%%\n", percent);
    lastpercent = percent;
    if (i + 1 < len && i % 77 == 0)
      sprintf(piece, "%d,\n", wavBuf[i]);
    else if (i + 1 < len)
      sprintf(piece, "%d,", wavBuf[i]);
    else
      sprintf(piece, "%d }", wavBuf[i]);
    piecelen = mystrnlen(piece, 255);
    strncat(contents, piece, piecelen + 1 + ((i + 1 == len) ? 1 : 0) + ((i % 77 == 0) ? 1 : 0));
    numtowrite = mystrnlen(contents, contentslen);
    totalwritten = 0;
    while(numtowrite > 0) {
      numwritten = fwrite(&(contents[totalwritten]), sizeof(char), numtowrite, f);
      if (ferror(f)) {
        fprintf(stderr, "Write error. Output file garbled. Exiting.\n");
        fclose(f);
        exit(1);
      }
      totalwritten += numwritten;
      numtowrite -= numwritten;
    }
  }
  contents[0] = '\0';
  strncat(contents, ";\n", 2);
  numtowrite = 2; 
  numwritten = fwrite(&(contents[0]), sizeof(char), numtowrite, f);
  fclose(f);
  return 0;
} 
size_t mystrnlen(char *s, size_t maxlen) {
  size_t i = 0;
  for (i = 0; i < maxlen; ++i) {
    if (s[i] == '\0')
      return i;
  }
  if (i == 0)
    return 0;
  return maxlen;
}
