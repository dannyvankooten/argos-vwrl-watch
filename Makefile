CFLAGS=-Wall -pedantic -g -Werror
LIBS= -L/usr/lib -lcurl 
PREFIX=/usr/local

vwrlwatch: vwrlwatch.c
	$(CC) $(CFLAGS) vwrlwatch.c -O3 -o $@ $(LIBS)

.PHONY: clean
clean:
	rm vwrlwatch 

.PHONY: install
install: vwrlwatch 
	cp $< $(PREFIX)/bin/vwrlwatch
	cp $< $(HOME)/Documents/BitBar/vwrlwatch.1h

.PHONY: uninstall 
uninstall:
	rm $(PREFIX)/bin/vwrlwatch 
