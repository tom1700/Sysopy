INC=include
CFLAG=-I$(INC)
LIB=lib/
.PHONY: static
.PHONY: shared
.PHONY: dynamic
.PHONY: clean

static:
	gcc -o main src/main.c $(LIB)*.a $(CFLAG)
	./main

shared:
	gcc -o main src/main.c ./$(LIB)*.so $(CFLAG) 
	./main

dynamic:
	gcc -rdynamic -o main src/main2.c -ldl $(CFLAG)
	./main
clean:
	rm -f *.o