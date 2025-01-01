#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// great theory:
// https://www.petercollingridge.co.uk/blog/mathematics-toys-and-games/dobble/

const char *e[] = {"🍎", "🍌", "🍇", "🍒", "🍓", "🥝", "🍑", "🍍", "🥥",
                   "🥕", "🌻", "🌈", "⚽", "🏀", "🎸", "🎲", "🎯", "🧩",
                   "📚", "✂️",  "🖌️", "🔧", "🔑", "🔋", "💡", "📞", "🕰️",
                   "📷", "🎥", "🚗", "🚲", "🚀", "🎈", "🌟", "🔥", "⛄",
                   "🦋", "🐞", "🐟", "🐘", "🐱", "🐶", "🦊", "🐼", "🦄",
                   "🦉", "🌳", "🌵", "🏔️", "⚓", "🛸", "🎷", "📀", "💎",
                   "🔭", "🌌", "🐢", "🐍", "🦜", "🍤", "🎡", "🏖️", "📡"};
const char *C[999];  // full card deck
int N = 6;           // symbols on a card Junior:6, Full:8
#define SZ \
  (N * N - N + 1)  // number of cards in a deck for N symbols per card
                   //
                   // shuffle int array of n elements
void shuffle(int *a, int n) {
  for (int i = 0, j, t; i < n; i++)
    j = rand() % n, t = a[i], a[i] = a[j], a[j] = t;
}

// generate a proper deck of cards, N symbols each.
// any two cards have one, and only one symbol in common.
void gencards() {
  int n = N - 1;
  const char **c = C;
  for (int i = 0; i <= n; i++) *c++ = e[i];
  for (int j = 0; j < n; j++) {
    *c++ = e[0];
    for (int k = 0; k < n; k++) *c++ = e[n + 1 + n * j + k];
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      *c++ = e[i + 1];
      for (int k = 0; k < n; k++) *c++ = e[n + 1 + n * k + (i * k + j) % n];
    }
  }
  for (int i = 0; i < SZ; i++) {
    for (int j = 0; j < N; j++) {
      int k = rand() % N;
      const char *tmp = C[i * N + j];
      C[i * N + j] = C[i * N + k], C[i * N + k] = tmp;
    }
  }
}

// print i-th card from deck with some prefix
void printcard(const char *prefix, int i) {
  printf("%s", prefix);
  for (int j = 0; j < N; j++) printf("%s ", C[i * N + j]);
  printf("\n");
}

// return a symbol, common between i-th and j-th cards in deck
const char *match(int i, int j) {
  for (int x = 0; x < N; x++)
    for (int y = 0; y < N; y++)
      if (strcmp(C[i * N + x], C[j * N + y]) == 0) return C[i * N + x];
  return NULL;
}

// read a digit 1..n during the timeout (in 0.1s units)
int input(int n, int timeout) {
  struct termios orig, term;
  int k = -1;
  tcgetattr(STDIN_FILENO, &orig);
  term = orig;
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
  for (int i = 0; i < timeout; i++) {
    printf("\r%c Symbol (1-%d)? ", "\\|//-"[i % 4], n);
    fflush(stdout);
    struct timespec ts = {0, 100000000L};  // 0.1s
    nanosleep(&ts, NULL);
    fd_set readfds;
    struct timeval tv = {0, 0};
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
      char c;
      if (read(STDIN_FILENO, &c, 1) == 1 && c >= '1' && c <= '0' + n) {
        k = c - '0';
        break;
      }
    }
  }
  printf("\r                              \r");
  tcsetattr(STDIN_FILENO, TCSANOW, &orig);
  return k;
}

int main() {
  int deck[SZ], won = 0, top = 0;
  srand(time(0));
  gencards();
  for (int i = 0; i < SZ; i++) deck[i] = i;
  shuffle(deck, SZ);
  for (int i = 1; i < SZ; i++) {
    printcard("Top: ", deck[top]);
    printcard("You: ", deck[i]);
    const char *answer = match(deck[i], deck[top]);
    int sym = input(N, 100);
    if (sym > 0) {
      if (strcmp(answer, C[N * deck[i] + sym - 1]) == 0) {
        printf("Correct %s %s!\n\n", answer, C[N * deck[i] + sym - 1]);
        top = i;
        won++;
      } else {
        printf("Incorrect, it was %s\n\n", answer);
        top = i + 1;
        i++;
      }
    } else {
      printf("Too slow, it was %s\n\n", answer);
      top = i + 1;
      i++;
    }
  }
  if (won >= (SZ - 1) / 2)
    printf("You won: %d vs %d\n", won, SZ - won - 1);
  else
    printf("You lost: %d vs %d\n", won, SZ - won - 1);
  return 0;
}
