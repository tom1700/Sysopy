#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "inout.h"

void move_through_file(int record_length, int number, FILE*handle){
	fseek(handle,number*record_length,SEEK_CUR);
}

int sort_lib(char * filename, int record_length){
	FILE * handle = fopen(filename, "rwb+");
	if(handle==NULL){
		perror(filename);
		return 1;
	}
	char * record1 = malloc(record_length);
	char * record2 = malloc(record_length);
	long not_sorted;
	long eof;
	fseek(handle,0,SEEK_END);
	eof = ftell(handle);
	fseek(handle,0,SEEK_SET);
	not_sorted = ftell(handle);
	fread(record1,sizeof(char),record_length,handle);
	while(not_sorted<eof){
		fread(record2,sizeof(char),record_length,handle);
		not_sorted = ftell(handle);
		move_through_file(record_length,-1,handle);
		while(1){
			move_through_file(record_length,-1,handle);
			fread(record1,sizeof(char),record_length,handle);
			//Proper field found
			if(record1[0]<=record2[0]){
				fwrite(record2,sizeof(char),record_length,handle);
				break;
			}
			fwrite(record1,sizeof(char),record_length,handle);
			move_through_file(record_length,-2,handle);
			//Start of file reached
			if(ftell(handle)==0){
				fwrite(record2,sizeof(char),record_length,handle);
				break;
			}
		}
		fseek(handle,not_sorted,SEEK_SET);
	}
	fclose(handle);
	return 0;
}
int sort_sys(char * filename, int record_length){
	int handle = open(filename,O_RDWR);
	if(errno > 0){
		perror(filename);
		return 1;
	}
	char * record1 = malloc(record_length);
	char * record2 = malloc(record_length);
	long eof=lseek(handle,0,SEEK_END);
	long start=lseek(handle,0,SEEK_SET);
	long not_sorted=lseek(handle,record_length,SEEK_SET);
	long position;
	pread(handle, record1, record_length,0);
	while(not_sorted<eof){
		pread(handle, record2, record_length, not_sorted);
		position = not_sorted;
		not_sorted += record_length;
		while(1){
			position -= record_length;
			pread(handle, record1, record_length,position);
			position += record_length;
			//Proper field found
			if(record1[0]<=record2[0]){
				pwrite(handle, record2, record_length,position);
				break;
			}
			pwrite(handle, record1, record_length,position);
			position -= record_length;
			//Start of file reached
			if(position==start){
				pwrite(handle, record2, record_length,position);
				break;
			}
		}
	}
	close(handle);
	return 0;
}
int main(int argc, char **argv){
	char * file = argv[1];
	char * record_length = argv[2];
	char * fun_type = argv[3];
	if(strlen(file) == 0 || strlen(record_length)==0 || strlen(fun_type)==0){
		printf("Bad arguments\n");
		return 1;
	}
	if(strcmp(fun_type,"lib")==0){
		return sort_lib(file,atoi(record_length));
	}
	else if (strcmp(fun_type,"sys")==0){
		return sort_sys(file,atoi(record_length));
	}
	else{
		return 1;
	}
	return 0;
}