#include "main.h"

int main(int argv,char** argc){
	int fold = atoi(argc[1]);
	int counter = 0;
	int c;
	while (((c = getchar())) != EOF){
		counter++;
		if(islower(c))
			c = toupper(c);
		if(putchar(c) == EOF){
			perror("output error"); 
    		exit(1); 
    	}
    	if(counter==fold)
			putchar('\n');
		if(c == '\n')
			fflush(stdout);
		counter %= fold;
	}
	exit(0);
}