CC=gcc
CFLAGS=-Wall -pedantic -Werror -lcurl

vwrlwatch: vwrlwatch.c .dist
	$(CC) $(CFLAGS) vwrlwatch.c -O3 -o .dist/vwrlwatch

debug: vwrlwatch.c .dist
	$(CC) $(CFLAGS) vwrlwatch.c -g -o .dist/debug

.dist:
	mkdir -p .dist

clean:
	rm -r .dist
