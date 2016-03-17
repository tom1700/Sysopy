#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void lock_read(int handle, int offset){
	printf("\nLocking to read byte %d\n\n",offset);
	struct flock * f= malloc(sizeof(struct flock));
	f->l_type=F_RDLCK;
	f->l_start=offset;
	f->l_whence=SEEK_SET;
	f->l_len=1;
	fcntl(handle,F_SETLK,f);
	free(f);

}
void lock_write(int handle, int offset){
	printf("\nLocking to write byte %d\n\n",offset);
	struct flock * f = malloc(sizeof(struct flock));
	f->l_type=F_WRLCK;
	f->l_start=offset;
	f->l_whence=SEEK_SET;
	f->l_len=1;
	fcntl(handle,F_SETLK,f);
	free(f);
}
void print_locked(int handle){
	long eof=lseek(handle,0,SEEK_END);
	long current=0;
	printf("\nLocked bytes:\n");
	while(lseek(handle,current,SEEK_SET)!=eof){
		struct flock * f = malloc(sizeof(struct flock));
		f->l_type=F_RDLCK;
		f->l_start=current;
		f->l_whence=SEEK_SET;
		f->l_len=1;
		fcntl(handle,F_GETLK,f);
		if(f->l_type != F_UNLCK){
			printf("%d Read %d\n",current,f->l_pid);
		}
		free(f);
		f = malloc(sizeof(struct flock));
		f->l_type=F_WRLCK;
		f->l_start=current;
		f->l_whence=SEEK_SET;
		f->l_len=1;
		fcntl(handle,F_GETLK,f);
		if(f->l_type != F_UNLCK){
			printf("%d Write %d\n",current,f->l_pid);
		}
		free(f);
		current++;	
	}
	printf("\n");
}
void free_lock(int handle, int offset){
	printf("\nUnlocking byte %d\n\n",offset);
	struct flock * f = malloc(sizeof(struct flock));
	f->l_type=F_UNLCK;
	f->l_start=offset;
	f->l_whence=SEEK_SET;
	f->l_len=1;
	fcntl(handle,F_SETLK,f);
	free(f);
}
void get_char(int handle, int position){
	char c;
	pread(handle, &c, 1, position);
	printf("%c\n",c);
}
void set_char(int handle,int position, int c){
	pwrite(handle, &c, 1,position);
}
void print_menu(){
	printf("Lock read: r\n");
	printf("Lock write: w\n");
	printf("Print locked: p\n");
	printf("Free lock:f\n");
	printf("Get char:g\n");
	printf("Set char: s\n");
	printf("Quit: q\n");
}

int main(int argc, char **argv){
	char * filename = argv[1];
	char option;
	int position;
	char tmp;
	if(strlen(filename) == 0){
		printf("Bad arguments\n");
		return 1;
	}
	int handle = open(filename,O_RDWR);
	while(1){
		print_menu();
		scanf("\n%c",&option);
		switch(option){
			case 'r':
				printf("Write position to lock\n");
				scanf("%d",&position);
				lock_read(handle,position);
				break;
			case 'w':
				printf("Write position to lock\n");
				scanf("%d",&position);
				lock_write(handle,position);
				break;
			case 'p':
				print_locked(handle);
				break;
			case 'f':
				printf("Write position to unlock\n");
				scanf("%d",&position);
				free_lock(handle,position);
				break;
			case 'g':
				printf("Write number of byte to get\n");
				scanf("%d",&position);
				get_char(handle,position);
				break;
			case 's':
				printf("Write position and char to set\n");
				scanf("%d %c",&position,&tmp);
				set_char(handle,position,tmp);
				break;
			case 'q':
				close(handle);
				return 0;
		}
	}
}