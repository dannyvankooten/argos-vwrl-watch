vwrlwatch: vwrlwatch.c 
	mkdir -p .dist
	cc vwrlwatch.c -lcurl -O3 -o .dist/vwrlwatch

debug: vwrlwatch.c
	cc vwrlwatch.c -lcurl -g -o .dist/debug
