.PHONY:all

all: server client

server:server.c
	gcc -o $@ $^ -lrt

client:client.c
	gcc -o $@ $^ -lrt