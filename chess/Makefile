all:
	$(CC) -Wall -W -g -pedantic -std=c99 chess.c -o chess

fmt:
	clang-format -i chess.c

clean:
	rm -f chess

.PHONY: all clean fmt

