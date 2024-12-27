all:
	$(CC) -Wall -W -g -pedantic -std=c99 sudoku.c -o sudoku
	./sudoku

fmt:
	clang-format -i sudoku.c

clean:
	rm -f sudoku

.PHONY: all clean fmt
