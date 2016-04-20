.PHONY: all

all: reader writer test

reader: reader.c
	gcc -Wall -o $@ $^ -lrt -pthread

test: test.c
	gcc -Wall -o $@ $^ -lrt -pthread

writer: writer.c
	gcc -Wall -o $@ $^ -lrt -pthread