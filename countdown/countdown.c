#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 20

void shuffle(int *a, int n) {
  for (int i = 0, j, t; i < n; i++)
    j = rand() % n, t = a[i], a[i] = a[j], a[j] = t;
}

int calc(int *N, int size) {
  int stack[MAX_SIZE];
  int sp = 0;
  for (int i = 0; i < size; i++) {
    if (N[i] > 0) {
      if (N[i] <= 0) return -1;
      stack[sp++] = N[i];
    } else {
      if (sp < 2) return -1;
      int b = stack[--sp];
      int a = stack[--sp];
      int result;
      switch (N[i]) {
        case -1: result = a + b; break;
        case -2: result = a - b; break;
        case -3: result = a * b; break;
        case -4: result = (b == 0 || a % b != 0) ? -1 : a / b; break;
        default: return -1;
      }
      if (result <= 0) return -1;
      stack[sp++] = result;
    }
  }
  return (sp == 1) ? stack[0] : -1;
}

void print(int *p, int n) {
  for (int i = 0; i < n; i++) {
    if (p[i] > 0)
      printf("%d ", p[i]);
    else if (p[i] == -1)
      printf("+ ");
    else if (p[i] == -2)
      printf("- ");
    else if (p[i] == -3)
      printf("* ");
    else if (p[i] == -4)
      printf("/ ");
  }
  printf("\n");
}

int gen(int n, int b, int hint) {
  int r = 0;
  int N[MAX_SIZE];
  static const int BIG[4] = {25, 50, 75, 100};
  for (int i = 0; i < b; i++) N[i] = BIG[rand() % 4];
  for (int i = b; i < n; i++) N[i] = rand() % 10 + 1;
  for (int i = 0; i < n; i++) { printf("%d ", N[i]); }
  for (;;) {
    shuffle(N, n);
    for (int i = 0; i < (n - 1); i++) {
      const char OPS[] = "+-*/";
      N[n + i] = -(rand() % 4 + 1);
    }
    int r = calc(N, n * 2 - 1);
    if (r > 100 && r < 999) {
      printf("=> %d\n", r);
      if (hint) print(N, n * 2 - 1);
      return r;
    }
  }
}

// Recursive function to solve the countdown problem and track the closest
// result
int solve_r(int *nums, int n, int target, int *expr, int pos, int *best_diff,
            int *best_expr, int *best_pos) {
  if (n == 1) {
    int diff = abs(nums[0] - target);
    if (diff < *best_diff) {
      *best_diff = diff;
      memcpy(best_expr, expr, pos * sizeof(int));  // Save best expression
      *best_pos = pos;
      best_expr[pos] = nums[0];  // Save the final number
    }
    return (nums[0] == target);
  }

  // Try all pairs of numbers
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == j) continue;
      int a = nums[i], b = nums[j];
      int rest[MAX_SIZE], restIdx = 0;

      // Prepare remaining numbers
      for (int k = 0; k < n; k++)
        if (k != i && k != j) rest[restIdx++] = nums[k];

      // Try all operations
      int ops[] = {-1, -2, -3, -4};
      for (int op = 0; op < 4; op++) {
        if (ops[op] == -4 && (b == 0 || a % b != 0))
          continue;  // Skip invalid division
        int result;
        switch (ops[op]) {
          case -1: result = a + b; break;
          case -2: result = a - b; break;
          case -3: result = a * b; break;
          case -4: result = a / b; break;
        }
        if (result <= 0) continue;

        // Add result and operation to expression
        rest[restIdx] = result;
        expr[pos] = a;
        expr[pos + 1] = b;
        expr[pos + 2] = ops[op];

        // Recursively solve for remaining numbers
        if (solve_r(rest, restIdx + 1, target, expr, pos + 3, best_diff,
                    best_expr, best_pos))
          return 1;  // Exact solution found
      }
    }
  }
  return 0;
}

void do_solve(int *nums, int n, int target) {
  int expr[MAX_SIZE], best_expr[MAX_SIZE];
  int best_diff = INT_MAX, best_pos = 0;

  if (!solve_r(nums, n, target, expr, 0, &best_diff, best_expr, &best_pos)) {
    // If no exact solution, print the closest result
    printf("Closest solution (off by %d): ", best_diff);
    print(best_expr, best_pos + 1);
  } else {
    // Exact solution found
    printf("Exact solution: ");
    print(expr, best_pos + 1);
  }
}

int main(int argc, char *argv[]) {
  int opt;
  int seed = time(NULL), n = 6, b = 2, solve = 0, hint = 0;
  while ((opt = getopt(argc, argv, "n:b:s:a")) != -1) {
    switch (opt) {
      case 'n': n = atoi(optarg); break;      // Total numbers
      case 'b': b = atoi(optarg); break;      // Big numbers
      case 's': solve = atoi(optarg); break;  // Big numbers
      case 'a': hint = 1; break;              // Big numbers
      default:
        fprintf(stderr,
                "USAGE: %s [-n <nums>] [-b <big>] [-a] [-s <solve> <n1> <n2> "
                "...]\n",
                argv[0]);
        exit(1);
    }
  }
  srand(seed);
  if (!solve) {
    gen(n, b, hint);
  } else {
    int N[MAX_SIZE];
    int idx = 0;
    for (int i = optind; i < argc; i++) N[idx++] = atoi(argv[i]);
    do_solve(N, idx, solve);
  }
  return 0;
}
