.PHONY:all
all: main main2 main3 generator
main: main.c
	gcc -o $@ $^ -Wall -pthread
generator: generator.c
	gcc -o $@ $^ -Wall -pthread
main2: main2.c
	gcc -o $@ $^ -Wall -pthread
main3: main3.c
	gcc -o $@ $^ -Wall -pthread