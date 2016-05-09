#include "header.h"

int forks[5];
sem_t fork_sem[5];
sem_t butler;
pthread_t philosophers[5];

void take_fork(int n){
	sem_wait(&fork_sem[n]);
	forks[n] = 1;
}
void put_fork(int n){
	forks[n] = 0;
	sem_post(&fork_sem[n]);
}
void * philosopher(void * args){
	int *id2 = args;
	int id=*id2;
	while(1){
		sem_wait(&butler);
		printf("Philosopher %d gets permission\n",id);
		printf("Philosopher %d thinking\n",id);
		take_fork(id);
		take_fork((id+1)%5);
		printf("Philosopher %d eating\n",id);
		put_fork(id);
		put_fork((id+1)%5);
		sem_post(&butler);
		printf("Philosopher %d loses permission\n",id);
	}
}
int main(int argv,char ** argc){
	for(int i=0;i<5;i++){
		forks[i]=0;
		if(sem_init(&fork_sem[i], 0, 1)==-1){
			printf("Semaphore creation");
			exit(1);
		}
	}
	if(sem_init(&butler, 0, 4)==-1){
		printf("Semaphore creation");
		exit(1);
	}
	for(int i=0;i<5;i++){
		if(pthread_create(&philosophers[i], NULL, philosopher, &i)==-1){
			perror("Thread");
			exit(1);
		}
		sleep(1);
	}
	while(1){}
	return 0;
}