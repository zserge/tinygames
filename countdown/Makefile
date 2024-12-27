all:
	$(CC) -Wall -W -g -pedantic -std=c99 countdown.c -o countdown
	./countdown

fmt:
	clang-format -i countdown.c

clean:
	rm -f countdown

.PHONY: all clean fmt

