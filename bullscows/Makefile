all:
	$(CC) -Wall -W -g -pedantic -std=c99 bullscows.c -o bullscows
	./bullscows

fmt:
	clang-format -i bullscows.c

clean:
	rm -f bullscows

.PHONY: all clean fmt
