vwrlwatch: vwrlwatch.c 
	mkdir -p .dist
	cc vwrlwatch.c -lcurl -O3 -o .dist/vwrlwatch