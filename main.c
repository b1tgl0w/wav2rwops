#define _POSIX_C_SOURCE 1
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define contentslen 999999999

Uint8 *wavBuf;
char contents[contentslen];
size_t mystrnlen(char *s, size_t maxlen);
int main(int argc, char *argv[])
{
  Uint32 i = 0;
  Uint32 len;
  Uint32 wavelenstrlen = 0;
  char wavelenstr[255];
  FILE *f;
  size_t totalwritten = 0;
  size_t numtowrite = 0;
  size_t numwritten = 0;
  size_t namelen = 0;
  int lastpercent = -1;
  SDL_AudioSpec desiredSpec;
  SDL_AudioSpec obtainedSpec;
  desiredSpec.freq = 44100;
  desiredSpec.channels = 2;
  contents[0] = '\0';
  if (argc != 3) {
    fprintf(stderr, "Usage: ./wavToRwops in.wav out.c\n");
    exit(1);
  }
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_OpenAudio(&desiredSpec, &obtainedSpec);
  if(SDL_LoadWAV(argv[1], &obtainedSpec, &wavBuf, &len) == NULL) {
    fprintf(stderr, "Failed to load wav\n");
    SDL_Quit();
    exit(1);
  }
  f = fopen(argv[2], "w");
  if (f == NULL) {
    fprintf(stderr, "Failed to open output file\n");
    SDL_FreeWAV(wavBuf);
    SDL_Quit();
    exit(1);
  }
  namelen = mystrnlen(argv[1], 255);
  strncat(contents, "Uint8 ", 6);
  strncat(contents, argv[1], namelen-4);
  strncat(contents, "wav[", 4);
  sprintf(wavelenstr, "%d", len);
  wavelenstrlen = mystrnlen(wavelenstr, 255);
  strncat(contents, wavelenstr, wavelenstrlen);
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
