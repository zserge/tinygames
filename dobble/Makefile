all:
	$(CC) -Wall -W -g -pedantic -std=c99 dobble.c -o dobble
	./dobble

fmt:
	clang-format -i dobble.c

clean:
	rm -f dobble

.PHONY: all clean fmt


