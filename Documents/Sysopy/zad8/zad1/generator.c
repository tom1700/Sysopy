#include "main.h"

void generateRecord(int id, record * rec){
	rec->id = id;
	for(int i = 0; i < 10; i++){
		rec->text[i] = (rand()%('z'-'a'))+'a';
	}
	rec->text[10] = 0;
}
void clearBuf(record *rec){
	for(int i=0;i<RECSIZE-4;i++){
		rec->text[i]=0;
	}
}
int main(int argv, char ** argc){
	int amount = atoi(argc[2]);
	char * filename = argc[1];
	FILE * handle;
	record * rec = malloc(RECSIZE);


	if((handle = fopen(filename,"w+"))==NULL){
		perror("File open");
		exit(1);
	}
	srand(time(NULL));
	for(int i=1;i<=amount;i++){
		clearBuf(rec);
		generateRecord(i,rec);
		fwrite(rec, RECSIZE, 1, handle);
	}

	return 0;
}