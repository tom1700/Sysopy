#include "header.h"

int shmid;
sem_t * sem;
int * arr;

char * gettime(char * time_buff){
	time_t writetime;
	time(&writetime);
	strftime(time_buff, 15, "%H:%M:%S", localtime(&writetime));
	return time_buff;
}

void sig_handler(int signo){
	if(signo==SIGINT){
		if(munmap(arr, MEMSIZE)==-1){
			perror("semm open");
			exit(1);
		}
		if(sem_close(sem)==-1){
			perror("sem close");
			exit(1);
		}
		if(close(shmid)==-1){
			perror("shm close");
			exit(1);
		}
		exit(0);
	}
}

int main(int argv,char**argc){
	int searchnum = atoi(argc[1]);
	char * timebuff = malloc(sizeof(char)*16);
	if((shmid = shm_open(spacename, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))==-1){
		perror("shm open");
		exit(1);
	}
	if((sem = sem_open(semname, O_RDWR))==SEM_FAILED){
		perror("semm open");
		exit(1);
	}
	if((arr = (int *)mmap(NULL, ARRSIZE,PROT_READ|PROT_WRITE, MAP_SHARED ,shmid, 0)) == MAP_FAILED){
		perror("semm open");
		exit(1);	
	}
	signal(SIGINT,sig_handler);

	while(1){
		sem_wait(sem);
		gettime(timebuff);
		int counter = 0;
		for(int i = 0;i<ARRSIZE;i++){
			if(arr[i]==searchnum){
				counter++;
			}
		}
		printf("(%d %s) znalazłem %d liczb o wartości %d\n",getpid(),timebuff,counter,searchnum);
		sem_post(sem);
	}
}