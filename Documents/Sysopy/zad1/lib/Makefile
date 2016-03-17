CC = gcc
IDIR=../include
CFLAGS=-I$(IDIR)

all: libstlist.a libshlist.so clean

libshlist.so: list2.o
	gcc -shared -o $@ $^

libstlist.a: list.o
	ar rc $@ $^
	ranlib $@

list.o: list.c $(IDIR)/list_header.h
	gcc -c -o $@ $< $(CFLAGS)

list2.o: list.c $(IDIR)/list_header.h
	gcc -fPIC -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o