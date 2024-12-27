/*
 * Sudoku Generator
 *
 * Generates a fully solved Sudoku grid for 4x4, 6x6, or 9x9 variants, then
 * masks cells to create a playable puzzle. Masking ensures there is exactly one
 * unique solution. Uses recursive backtracking for solving and validating
 * grids.
 *
 * Command-line options:
 * -4 | -6 | -9       Select grid size (default: 9x9)
 * -m <hidden>        Specify number of cells to mask (default: 40)
 * -s <seed>          Seed for random number generator (default: current time)
 * -a                 Show solved grid before masking
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Shuffle array `a` of `n` elements
void shuffle(int *a, int n) {
  for (int i = 0, j, t; i < n; i++)
    j = rand() % n, t = a[i], a[i] = a[j], a[j] = t;
}

// Print grid `g` of `n` elements and `w` columns
void print(int *g, int n, int w) {
  for (int i = 0; i < n; i++)
    printf("%c%c", g[i] ? g[i] + '0' : '.', ((i + 1) % w ? ' ' : '\n'));
}

// Validate number `n` placement at [r, c] for grid `g` with width `w`
int valid(int *g, int w, int r, int c, int n) {
  int u, v;
  if (w == 9) u = v = 3;     // 3x3 blocks (9x9 Sudoku)
  if (w == 6) u = 3, v = 2;  // 3x2 blocks (6x6 Sudoku)
  if (w == 4) u = v = 2;     // 2x2 blocks (4x4 Sudoku)
  for (int i = 0; i < w; i++)
    if (g[r * w + i] == n || g[i * w + c] == n ||
        g[(r / v * v + i / u) * w + (c / u * u + i % u)] == n)
      return 0;
  return 1;
}

// Solve grid `g` of size `n` (width `w`), starting at `pos`, up to `maxcnt`
// solutions
int solve(int *g, int n, int w, int pos, int maxcnt) {
  if (pos == n) return 1;                              // Found a solution
  if (g[pos]) return solve(g, n, w, pos + 1, maxcnt);  // Skip pre-filled cells

  int r = pos / w, c = pos % w, solutions = 0;
  int N[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffle(N, w);  // Randomise numbers for diversity

  for (int i = 0; i < w; i++) {
    if (valid(g, w, r, c, N[i])) {
      g[pos] = N[i];
      solutions += solve(g, n, w, pos + 1, maxcnt);
      if (solutions >= maxcnt) break;  // Stop early if maxcnt reached
      g[pos] = 0;                      // Backtrack
    }
  }
  return solutions;
}

// Mask `hide` cells in grid `g` of size `n` (width `w`), ensuring uniqueness
void mask(int *g, int n, int w, int hide) {
  int positions[81], tmp[81];
  for (int i = 0; i < n; i++) positions[i] = i;  // Index grid positions
  shuffle(positions, n);                         // Randomise cell removal order

  int attempts = 0;
  for (; hide > 0 && attempts < n; attempts++) {
    int pos = positions[attempts];
    if (g[pos] == 0) continue;  // Skip already masked cells
    int backup = g[pos];
    g[pos] = 0;                                 // Temporarily remove cell
    for (int i = 0; i < n; i++) tmp[i] = g[i];  // Create working copy
    if (solve(tmp, n, w, 0, 2) != 1) g[pos] = backup;  // Rollback if not unique
    else hide--, attempts = 0;
  }
}

int main(int argc, char *argv[]) {
  int opt;
  int hide = 40, cols = 9, size = 81, seed = time(0), answer = 0;
  int grid[81] = {0};
  while ((opt = getopt(argc, argv, "m:s:a469")) != -1) {
    switch (opt) {
      case 'a': answer = 1; break;           // Show answer before masking
      case 's': seed = atoi(optarg); break;  // Set PRNG seed
      case 'm': hide = atoi(optarg); break;  // Number of cells to mask
      case '4': cols = 4, size = 16; break;  // Use 4x4 grid
      case '6': cols = 6, size = 36; break;  // Use 6x6 grid
      case '9': cols = 9, size = 81; break;  // Use 9x9 grid
      default:
        fprintf(stderr, "USAGE: %s [-4|-6|-9] [-a] [-m <masked>] [-s <seed>]\n",
                argv[0]);
        exit(1);
    }
  }
  srand(seed);
  solve(grid, size, cols, 0, 1);  // Generate full grid
  if (hide && answer) {
    print(grid, size, cols);  // Print solution if requested
    printf("\n");
  }
  mask(grid, size, cols, hide);  // Mask cells
  print(grid, size, cols);       // Final puzzle
  return 0;
}
