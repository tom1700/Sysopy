#include "header.h"
int shmid;
sem_t * sem;
sem_t * bsem;
int * writerspid;
int * readerspid;
int writersamount;
int readersamount;


void sig_handler(int signo){
	if(signo==SIGINT){
		for(int i=0;i<writersamount;i++){
			kill(writerspid[i],SIGINT);
		}
		for(int i=0;i<readersamount;i++){
			kill(readerspid[i],SIGINT);
		}
		free(writerspid);
		free(readerspid);
		if(sem_close(sem)==-1){
			perror("sem close");
			exit(1);
		}
		if(sem_unlink(semname)==-1){
			perror("sem unlink");
			exit(1);
		}
		if(sem_close(bsem)==-1){
			perror("sem close");
			exit(1);
		}
		if(sem_unlink(bsemname)==-1){
			perror("sem unlink");
			exit(1);
		}
		if(close(shmid)==-1){
			perror("shm close");
			exit(1);
		}
		if(shm_unlink(spacename)==-1){
			perror("shm unlink");
			exit(1);
		}
		exit(0);
	}
}
//argc[1] number of writers processess to start, argc[2] number of readers processess to start
int main(int argv,char**argc){
	char **wargs = malloc(sizeof(char*)*3);
	char **rargs = malloc(sizeof(char*)*3);
	writersamount = atoi(argc[1]);
	readersamount = atoi(argc[2]);
	writerspid = malloc(sizeof(char)*writersamount);
	readerspid = malloc(sizeof(char)*readersamount);

	signal(SIGINT,sig_handler);
	srand(time(NULL));

	wargs[0]=malloc(sizeof(char)*7);
	rargs[0]=malloc(sizeof(char)*7);
	rargs[1]=malloc(sizeof(char)*12);
	strcpy(wargs[0],"writer");
	strcpy(rargs[0],"reader");
	wargs[1]=argc[2];
	rargs[2]=argc[2];
	wargs[2]=NULL;
	rargs[2]=NULL;

	if((shmid = shm_open(spacename, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))==-1){
		perror("shm open");
		exit(1);
	}
	ftruncate(shmid, MEMSIZE);
	if((sem = sem_open(semname, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, readersamount))==SEM_FAILED){
		perror("sem open");
		exit(1);
	}
	if((bsem = sem_open(bsemname, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 1))==SEM_FAILED){
		perror("bin sem open");
		exit(1);
	}
	for(int i = 0;i < writersamount;i++){
		int pid = fork();
		if(pid==0){
			execv(wargs[0],wargs);
		}
		writerspid[i] = pid;
	}
	for(int i=0;i<readersamount;i++){
		int pid = fork();
		int n = rand()%ARRSIZE;
		sprintf(rargs[1],"%d",n);
		if(pid==0){
			execv(rargs[0],rargs);
		}
		readerspid[i] = pid;
	}
	while(1){}
}