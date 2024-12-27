#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define L 64

int main(int c, char **v) {
  int w = 5, t = 6, o;
  char x[L], g[L], *f = "WORDS.txt";
  while ((o = getopt(c, v, "w:d:t:")) != -1) {
    switch (o) {
      case 'w': w = atoi(optarg); break;
      case 't': t = atoi(optarg); break;
      case 'd': f = optarg; break;
    }
  }
  if (w < 1 || w >= L) return fprintf(stderr, "invalid length: %d\n", w), 1;

  FILE *d = fopen(f, "r");
  if (!d) return perror(f), 1;

  srand(time(0));
  int n = 0;
  while (fgets(x, L, d)) {
    int l = strlen(x);
    if (x[l - 1] == '\n') x[--l] = 0;
    if (l == w && islower(*x) && rand() % ++n == 0) strcpy(g, x);
  }
  fclose(d);
  if (!n) return fprintf(stderr, "No words of length %d\n", w), 1;

  printf("Guess the %d-letter word!\n", w);
  for (; t > 0; t--) {
    printf("%d tries left: ", t);
    if (!fgets(x, L, stdin)) break;
    int l = strlen(x);
    if (x[l - 1] == '\n') x[--l] = 0;
    if (l != w) {
      printf("Must be %d letters.\n", w);
      t++;
      continue;
    }

    for (int i = 0; i < w; i++) x[i] = tolower(x[i]);
    if (!strcmp(x, g)) return printf("Correct! The word was %s.\n", g), 0;

    for (int i = 0; i < w; i++) {
      if (x[i] == g[i])
        putchar('#');
      else if (strchr(g, x[i]))
        putchar('+');
      else
        putchar('.');
    }
    putchar('\n');
  }
  printf("Out of tries! The word was %s.\n", g);
  return 0;
}
