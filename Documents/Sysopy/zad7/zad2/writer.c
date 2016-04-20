#include "header.h"

int shmid;
int * arr;
sem_t * sem;
sem_t * bsem;

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
		if(sem_close(bsem)==-1){
			perror("bsem close");
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
	int readersamount = atoi(argc[1]);
	char * timebuff = malloc(sizeof(char)*16);

	if((shmid = shm_open(spacename, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))==-1){
		perror("shm open");
		exit(1);
	}
	if((sem = sem_open(semname, O_RDWR))==SEM_FAILED){
		perror("semm open");
		exit(1);
	}
	if((bsem = sem_open(bsemname, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1))==SEM_FAILED){
		perror("bin sem open");
		exit(1);
	}
	if((arr = (int *)mmap(NULL, ARRSIZE,PROT_READ|PROT_WRITE, MAP_SHARED ,shmid, 0)) == MAP_FAILED){
		perror("semm open");
		exit(1);	
	}

	signal(SIGINT,sig_handler);

	while(1){
		sem_wait(bsem);
		for(int i=0;i<readersamount;i++)
			sem_wait(sem);
		int tasks = rand()%ARRSIZE;
		while(tasks >= 0){
			int p = (rand()%ARRSIZE);
			int x = rand()%ARRSIZE;
			arr[p] = x;
			printf("(%d %s) Wpisałem liczbę %d - na pozycję %d. Pozostało %d zadań\n",getpid(),gettime(timebuff),x,p,tasks);
			tasks--;
		}
		for(int i=0;i<readersamount;i++){
			sem_post(sem);
		}
		sem_post(bsem);
	}
}