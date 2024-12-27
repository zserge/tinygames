all:
	$(CC) -Wall -W -g -pedantic -std=c99 wordle.c -o wordle
	./wordle

fmt:
	clang-format -i wordle.c

clean:
	rm -f wordle

.PHONY: all clean fmt


