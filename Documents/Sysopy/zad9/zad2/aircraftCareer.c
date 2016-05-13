#include "header.h"


int N; //ship capacity
int K; //edge amount
int A; //number of aircrafts
int wants_to_land;
int wants_to_start;
int landed;
int isFree;
sem_t access_sem;
sem_t lane_for_start;
sem_t lane_for_landing;
pthread_t * aircrafts;


void free_lane(){
	if(landed < K){
		if(wants_to_land > 0){
			wants_to_land--;
			sem_post(&lane_for_landing);
		}
	 	else if(wants_to_start > 0){
	 		wants_to_start--;
	 		sem_post(&lane_for_start);
	 	}
	 	else{ 
	 		isFree = 1;
	 	}
	}
	else{
		if(wants_to_start > 0){
			wants_to_start--;
			sem_post(&lane_for_start);
		}
		else if((wants_to_land > 0) && (landed < N)){
			wants_to_land--;
			sem_post(&lane_for_landing);
		}
		else{
			isFree = 1;
		}
	}
}

void fly(){
	sleep((rand()%4)+1);
}

void land(){
	sem_wait(&access_sem);
 	if((isFree==1) && (landed < N)){
 		isFree = 0;
 		sem_post(&access_sem);
 	}
 	else{
 		wants_to_land++;
 		sem_post(&access_sem);
 	    sem_wait(&lane_for_landing);
 	}
 	sem_wait(&access_sem);
 	landed++;
 	free_lane();
 	sem_post(&access_sem);
}

void rest(){
	sleep((rand()%4)+1);
}

void start(){
	sem_wait(&access_sem);
 	if (isFree==1){
 		isFree = 0;
 		sem_post(&access_sem);
 	}
 	else{
 		wants_to_start++;
 		sem_post(&access_sem);
 		sem_wait(&lane_for_start);
 	}
 	sem_wait(&access_sem);
 	landed--;
 	free_lane();
 	sem_post(&access_sem);
}

void * aircraft(void * args){
	int *id2 = args;
	int id=*id2;
	while(1){
		printf("Aircraft %d is flying\n",id);
		fly();
		printf("Aircraft %d is landing\n",id);
		land();
		printf("Aircraft %d is resting\n",id);
		rest();
		printf("Aircraft %d is starting\n",id);
		start();
	}
}

void sig_handle(int signum){
	for(int i=0;i<5;i++){
		pthread_cancel(aircrafts[i]);
	}
	sem_destroy(&access_sem);
	sem_destroy(&lane_for_start);
	sem_destroy(&lane_for_landing);
	exit(0);
}

int main(int argv,char ** argc){
	N = atoi(argc[1]);
	K = atoi(argc[2]);
	A = atoi(argc[3]);
	wants_to_land=0;
	wants_to_start=0;
	landed=0;
	isFree = 1;
	signal(SIGINT,sig_handle);
	if(sem_init(&access_sem, 0, 1)==-1){
		printf("Semaphore creation");
		exit(1);
	}
	if(sem_init(&lane_for_start, 0, 1)==-1){
		printf("Semaphore creation");
		exit(1);
	}
	if(sem_init(&lane_for_landing, 0, 1)==-1){
		printf("Semaphore creation");
		exit(1);
	}
	srand(time(NULL));
	aircrafts = malloc(A * sizeof(pthread_t));
	for(int i=0;i<A;i++){
		if(pthread_create(&aircrafts[i], NULL, aircraft, &i)==-1){
			perror("Thread");
			exit(1);
		}
	}
	while(1){}
	return 0;
}