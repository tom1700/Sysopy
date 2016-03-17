#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int generate(char * filename,int record_length,int number_of_records){
	FILE * handle = fopen(filename, "ab+");
	if(handle==NULL){
		perror(filename);
		return 1;
	}
	char * record = malloc(record_length);
	for(int i=0;i<number_of_records;i++){
		for(int j=0;j<record_length;j++){
			record[j]=rand();
		}
		fwrite(record,sizeof(char),record_length,handle);
	}
	return 0;
}
int main(int argc, char **argv){
	char * file = argv[1];
	char * record_length = argv[2];
	char * number_of_records = argv[3];
	if(strlen(file) == 0 || strlen(record_length)==0 || strlen(number_of_records)==0){
		printf("Bad arguments\n");
		return 1;
	}
	srand(time(NULL));
	generate(file,atoi(record_length),atoi(number_of_records));
	return 0;
}