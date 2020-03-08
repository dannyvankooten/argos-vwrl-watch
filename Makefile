CFLAGS=-Wall -pedantic -g -Werror
LIBS= -lcurl 
PREFIX=/usr/local

vwrlwatch: vwrlwatch.c
	$(CC) $(CFLAGS) vwrlwatch.c -O3 -o $@ $(LIBS)

.PHONY: clean
clean:
	rm vwrlwatch 

.PHONY: install
install: vwrlwatch 
	cp $< $(PREFIX)/bin/vwrlwatch

.PHONY: uninstall 
uninstall:
	rm $(PREFIX)/bin/vwrlwatch 
