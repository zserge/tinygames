#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

//
// board representation:
//
// +-------------------+-------+
// | 00 01 02 03 04 05 | 06 07 |
// | 08 09 10 11 12 13 | 14 15 |
// | 16 17 18 19 20 21 | 22 23 |
// | 24 25 26 27 28 29 | 30 31 |
// | 32 33 34 35*36 37 | 38 39 |
// | 40 41 42 43 44 45 | 46 47 |
// +-------------------+-------+
//
// pieces notation PNKBRQ + . (empty)
//   N[p+6]   piece symbol
//   p>0      player's piece
//   p<0      opponent's piece
//   p>3      sliding piece
//   !p       empty square
//   -p       toggle side (player/opponent)
char *N = "qrbknp.PNKBRQ";

// W/H - width/height, E - bottom-right (end) square
// B: board with 6 rows and sentinel area on the right
// M: moves storage (stack-like, for recursive negamax), m - move stack pointer
int W, H, E, B[48], M[9999], *m, Q=6;

// piece step vectors: +1/-1 = horisontal, +8/-8 = vertical, -7/-9/+7/+9 = diagonal
// vectors must be null-terminated
int D[5][9] = {
	{-17, - 15, -6, -10, 6, 10, 15, 17, 0}, // N
	{-7, -8, -9, -1, 1, 7, 8, 9, 0}, // K
	{-7, -9, 7, 9, 0}, // B
	{-8, -1, 1, 8, 0}, // R
	{-7, -8, -9, -1, 1, 7, 8, 9, 0}, // Q
};

// print board
void pr(void) { for (int i = 0; i < W*H; i++) printf("%c%c", N[B[i/W*8+i%W]+6], (i+1)%W?' ':'\n'); }
// parse FEN (assuming it's valid)
void fen(char *s) {
	for (int i = 0; *s && *s != ' '; s++)
		if (*s == '/')  i = (i/8+1)*8;
		else if (*s > 'A') B[i++] = strchr(N, *s) - N - 6;
		else if (*s > '0') i += *s - '0';
}
// flip board (rotate 180 degrees) and swap sides
void flip(void) { for (int i=0,j=E-1,t; i<(E+1)/2; i++,j--) t=B[i], B[i]=-B[j], B[j]=-t; }
// appends all valid player moves to the end of array M(m)
void moves(void) {
	for (int i = 0, p; i < E; i++) { // check every square in the playing area
		if ((p = B[i]) > 0) { // player's piece?
			if (p > 1) { // not a pawn?
				for (int *d = D[p-2], to, step; *d; d++) {
					for (step = 0, to=i+*d; step < (p>3?6:1); to+=*d,step++) {
						if (to < 0 || to >= E || to % 8 >= W || B[to] > 0) break; // invalid or player's piece
						*m++ = (i<<8)|to; // store valid move (from+to)
						if (B[to] < 0) break; // capture, stop sliding
					}
				}
			} else { // pawn: very different form other pieces
				if (i>8 && B[i-9]<0) *m++ = (i<<8)|(i-9); // capture up+left
				if (i>7 && B[i-7]<0) *m++ = (i<<8)|(i-7); // capture up+rigth
				if (i>8 && !B[i-8]) *m++ = (i<<8)|(i-8);  // move ahead (+promition)
			}
		}
	}
}
// current board score
int eval(void) {
	int V[]={-9,-5,-3,-999,-3,-1,0,1,3,999,3,5,9}, score = 0;
	for (int i = 0; i < E; i++) score += V[B[i]+6];
	return score;
}
// apply move to the board, return captured piece, if any
int move(int m) {
	int from = m >> 8, to = m & 0xff, capture = B[to];
	// TODO: handle pawn promotion to `int Q=6` piece 
	B[to] = B[from]; B[from] = 0;
	return capture;
}

int negamax(int depth, int *best) {
	if (!depth) return eval();
	int *start = m, *end, max = -999, tmp;
	moves();
	end = m;
	for (int *n = start; n < end; n++) {
		int captured = move(*n), score;
		flip();
		score = -negamax(depth - 1, &tmp);
		flip(); 
		B[*n>>8] = B[*n&0xff], B[*n&0xff] = captured;
		if (score >= max) max = score, *best = *n;
	}
	m = start;
	return max;
}

int main(int argc, char *argv[]) {
	int opt, mode = 6;
  while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
			case 'n': mode = atoi(optarg); break;
      default:
        fprintf(stderr, "USAGE: %s [-n 4|5|6]\n",
                argv[0]);
        return 1;
		}
	}

	if (mode == 6) W = 6, H = 6, E=46, fen("rnqknr/pppppp/6/6/PPPPPP/RNQKNR");
	else if (mode == 4) W = 4, H = 5, E=36, fen("kbnr/p3/4/3P/RNBK");
	else if (mode == 5) W = 5, H = 5, E=37, fen("rnbqk/ppppp/5/PPPPP/RNBQK");
	else return fprintf(stderr, "invalid mode: %d\n", mode);

	pr();

	for (;;) {
		char c1, c2;
		int r1, r2, n, *u, best = 0;
		m = M;
		moves(); // generate valid moves
		for (;;) {
			if (scanf("%c%d%c%d", &c1, &r1, &c2, &r2) != 4) continue; // user input, i.e. e2e4
			n = ((8*(H-r1) + c1 - 'a') << 8) | (8*(H-r2)+c2-'a');
			for (u = M; u < m; u++) if (*u == n) break;
			if (u != m) break;
			printf("invalid move\n");
		}
		move(n);
		flip();
		negamax(2, &best);
		if (best == 0 || eval() < -100 || eval() > 100) {
			printf("GAME OVER: %d\n", eval());
			return 0;
		}
		move(best);
		flip();
		printf("opponent: %c%d%c%d (eval %d)\n",
               'a' + (best >> 8) % 8, 8 - (best >> 8) / 8,
               'a' + (best & 0xff) % 8, 8 - (best & 0xff) / 8, eval());
		pr();
	}
}
