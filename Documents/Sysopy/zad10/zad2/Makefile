.Phony:all

all:server client

server:server.c
	gcc -o $@ $^ -lpthread -Wall

client:client.c
	gcc -o $@ $^ -lpthread -Wall