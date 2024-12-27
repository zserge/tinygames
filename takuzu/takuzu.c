/*
 * Takuzu Generator
 *
 * Takuzu is a binary puzzle where the grid consists of `0`s and `1`s.
 * The rules for a valid Takuzu grid are:
 * 1. Each row and column must have an equal number of `0`s and `1`s.
 * 2. No more than two consecutive `0`s or `1`s are allowed in any row or
 * column.
 * 3. All rows and columns must be unique.
 *
 * This program generates a valid Takuzu puzzle with a unique solution.
 *
 * Command-line arguments:
 * -n <size>    Grid size (even number between 2 and 20, default: 8).
 * -m <masked>  Number of cells to mask in the final puzzle (default: 20).
 * -s <seed>    Random seed for reproducibility (default: current time).
 * -a           Print the solved grid (answer) before masking cells.
 *
 * Example:
 * ./takuzu -n 10 -m 30 -s 1234 -a
 * Generates a 10x10 puzzle with 30 masked cells, using seed 1234, and prints
 * the solved grid before displaying the masked puzzle.
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 20

// Shuffle array `a` of `n` elements
void shuffle(int *a, int n) {
  for (int i = 0, j, t; i < n; i++)
    j = rand() % n, t = a[i], a[i] = a[j], a[j] = t;
}

// Print the Takuzu grid
void print(int *g, int n) {
  for (int i = 0; i < n * n; i++)
    printf("%c%c", g[i] < 0 ? '.' : g[i] + '0', (i + 1) % n ? ' ' : '\n');
}

// Compare two sequences (rows/columns) for equality
int eq(int *g, int a, int b, int step, int n) {
  for (int i = 0; i < n; i++)
    if (g[a + i * step] != g[b + i * step]) return 0;
  return 1;
}

// Check if placing `val` at position [r, c] is valid
int valid(int *g, int w, int r, int c, int v) {
  // Check for no three consecutive values in row or column
  if ((r >= 2 && g[(r - 2) * w + c] == v && g[(r - 1) * w + c] == v) ||
      (c >= 2 && g[r * w + c - 2] == v && g[r * w + c - 1] == v))
    return 0;
  int nr = 0, nc = 0;
  for (int i = 0; i < w; i++) nr += g[r * w + i] == v, nc += g[i * w + c] == v;
  if (nr > w / 2 || nc > w / 2) return 0;

  // Check for uniqueness in row and column
  g[r * w + c] = v;
  for (int i = 0; i < r; i++)
    if (eq(g, i * w, r * w, 1, w)) return g[r * w + c] = -1, 0;
  for (int i = 0; r == w - 1 && i < c; i++)
    if (eq(g, i, c, w, w)) return g[r * w + c] = -1, 0;
  return 1;
}

// Recursively solve or fill the Takuzu grid
int solve(int *g, int n, int pos, int maxcnt) {
  if (pos == n * n) return 1;  // Found a solution
  if (g[pos] != -1)
    return solve(g, n, pos + 1, maxcnt);  // Skip pre-filled cells

  int solutions = 0, r = pos / n, c = pos % n;

  int xor = rand() & 1;
  for (int i = 0; i < 2; i++) {
    if (valid(g, n, r, c, i ^ xor)) {
      solutions += solve(g, n, pos + 1, maxcnt);
      if (solutions >= maxcnt) break;  // Stop early if maxcnt reached
      g[pos] = -1;                     // Backtrack
    }
  }
  return solutions;
}

// Mask cells while ensuring unique solution
void mask(int *g, int n, int mask_count) {
  int positions[MAX_SIZE * MAX_SIZE], tmp[MAX_SIZE * MAX_SIZE];
  for (int i = 0; i < n * n; i++) positions[i] = i;
  shuffle(positions, n * n);

  int attempts = 0;
  for (; mask_count > 0 && attempts < n * n; attempts++) {
    int pos = positions[attempts];
    if (g[pos] == -1) continue;  // Skip already masked cells
    int backup = g[pos];
    g[pos] = -1;                          // Temporarily mask cell
    memcpy(tmp, g, sizeof(int) * n * n);  // Copy grid for testing
    if (solve(tmp, n, 0, 2) != 1)
      g[pos] = backup;  // Rollback if not unique
    else
      mask_count--, attempts = 0;  // Reset attempts on success
  }
}

int main(int argc, char *argv[]) {
  int opt, size = 8, mask_count = 20, seed = time(0), answer = 0;
  int grid[MAX_SIZE * MAX_SIZE];
  memset(grid, -1, sizeof(grid));

  // Parse command-line arguments
  while ((opt = getopt(argc, argv, "n:m:s:a")) != -1) {
    switch (opt) {
      case 'n': size = atoi(optarg); break;        // Grid size
      case 'm': mask_count = atoi(optarg); break;  // Cells to mask
      case 's': seed = atoi(optarg); break;        // Random seed
      case 'a': answer = 1; break;  // Print answer before masking
      default:
        fprintf(stderr,
                "USAGE: %s [-n <size>] [-m <masked>] [-s <seed>] [-a]\n",
                argv[0]),
            exit(1);
    }
  }

  if (size % 2 != 0 || size > MAX_SIZE) {
    fprintf(stderr, "Error: size must be even and <= %d\n", MAX_SIZE);
    return 1;
  }

  srand(seed);              // Seed RNG
  solve(grid, size, 0, 1);  // Generate fully solved grid

  if (answer) {
    print(grid, size);  // Print solution if requested
    printf("\n");
  }

  mask(grid, size, mask_count);  // Mask cells
  print(grid, size);             // Print final puzzle

  return 0;
}
