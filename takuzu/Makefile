all:
	$(CC) -Wall -W -g -pedantic -std=c99 takuzu.c -o takuzu
	./takuzu

fmt:
	clang-format -i takuzu.c

clean:
	rm -f takuzu

.PHONY: all clean fmt

