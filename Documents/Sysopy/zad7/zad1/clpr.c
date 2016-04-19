#include "header.h"
//arg[1] path to shared memory, arg[2] some sunsigned int, arg[3] 'c' for customer 'p' for producer
//Data structure: [0] amount of numbers in array [1] put index [2] take index 
char * gettime(char * time_buff){
	time_t writetime;
	time(&writetime);
	int pid = getpid();
	strftime(time_buff, 15, "%H:%M:%S", localtime(&writetime));
	return time_buff;
}
void semup(int semid){
	struct sembuf sops[2];
    sops[0].sem_num = 0;        // Operate on semaphore 0
    sops[0].sem_op = 0;         // Wait for value to equal 0
    sops[0].sem_flg = 0;
    sops[1].sem_num = 0;        // Operate on semaphore 0
    sops[1].sem_op = 1;         // Increment value by one
    sops[1].sem_flg = 0;
    if (semop(semid, sops, 2) == -1) {
        perror("semoperation");
        exit(EXIT_FAILURE);
    }
}

void semdown(int semid){
	struct sembuf sops[1];
    sops[0].sem_num = 0;        // Operate on semaphore 0
    sops[0].sem_op = -1;         // Decrement value by one
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1) {
        perror("semoperation");
        exit(EXIT_FAILURE);
    }
}
void producer(int shmid, int semid, int * shm_address){
	char * timebuff = malloc(sizeof(char)*16);
	int n;
	while(1){
		n = rand();
		while(1){
			semup(semid);
			//If array is full let other processes try
			if(shm_address[0]<SHMARRSIZE){
				shm_address[shm_address[1]]=n;
				shm_address[1]=((shm_address[1]-2)%SHMARRSIZE)+3;
				shm_address[0]++;
				printf("(%d %s) Dodałem liczbę: %d. Liczba zadań oczekujących: %d.\n",getpid(),gettime(timebuff),n,shm_address[0]);
				break;
			}
			semdown(semid);
		}
		semdown(semid);
	}
}

void consumer(int shmid, int semid, int * shm_address){
	char * timebuff = malloc(sizeof(char)*16);
	int n,i;
	while(1){
		semup(semid);
		if(shm_address[0]>0){
			n = shm_address[shm_address[2]];
			i = shm_address[2]; 
			shm_address[2]=((shm_address[2]-2)%SHMARRSIZE)+3;
			shm_address[0]--;
			if(n%2==0)
				printf("(%d %s) Sprawdziłem liczbę %d na pozycji %d - parzysta. Pozostało zadań oczekujących: %d.\n",getpid(),gettime(timebuff),n,i,shm_address[0]);
			else
				printf("(%d %s) Sprawdziłem liczbę %d na pozycji %d - nieparzysta. Pozostało zadań oczekujących: %d.\n",getpid(),gettime(timebuff),n,i,shm_address[0]);
		}
		semdown(semid);
	}
}

int main(int argv,char**argc){
	key_t key;
	int shmid;
	int semid;
	int *shm_address;
	srand(time(NULL));
	if((key = ftok(argc[1], atoi(argc[2])))==-1){
    	perror("Key error");
		exit(1);
    }
	if((shmid = shmget(key, SHMSIZE,ALL_PERM))==-1){
		perror("Creating queue error");
		exit(1);
	}
	if((semid = semget(key, NUMSEMS, ALL_PERM)) == -1){
        perror("Creating semaphore");
		exit(1);
    }
    if((shm_address = (int *)shmat(shmid, NULL, 0))==NULL ){
        perror("Attaching shared memory");
		exit(1);
    }

	if(argc[3][0]=='c')
		producer(shmid, semid, shm_address);
	if(argc[3][0]=='p')
		consumer(shmid, semid, shm_address);
	return 0;
}