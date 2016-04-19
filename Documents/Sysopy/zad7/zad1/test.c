#include "header.h"
//ctr^c to stop and kill all child processess
//arg[1] path to empty file without extension, arg[2] some sunsigned int, arg[3] number of producers,arg[4] number of consumers
int consnum;
int prodnum;
int * prodpid;
int * conpid;
int shmid;
int semid;

void sig_handle(int signo){
	if(signo==SIGINT){
    	for(int i=0;i<consnum;i++){
			kill(prodpid[i],SIGINT);
		}
		for(int i=0;i<prodnum;i++){
			kill(conpid[i],SIGINT);
		}
		if(shmctl(shmid,IPC_RMID, 0) == -1){
        	perror("Closing error");
        	exit(1);
    	}
    	if(semctl(semid,IPC_RMID, 0) == -1){
        	perror("Closing error");
        	exit(1);
    	}
		exit(0);
	}
}
int main(int argv,char**argc){
	key_t key;
	int pid;
	char **cargs = malloc(sizeof(char*)*5);
	char **pargs = malloc(sizeof(char*)*5);
	int *shm_address;
	consnum = atoi(argc[3]);
	prodnum = atoi(argc[4]);
	prodpid = malloc(sizeof(int)*prodnum);
	conpid = malloc(sizeof(int)*consnum);

	cargs[0]=malloc(sizeof(char)*5);
	pargs[0]=malloc(sizeof(char)*5);
	cargs[1]=malloc(strlen(argc[1]));
	pargs[1]=malloc(strlen(argc[1]));
	cargs[2]=malloc(strlen(argc[2]));
	pargs[2]=malloc(strlen(argc[2]));
	cargs[3]=malloc(sizeof(char)*2);
	pargs[3]=malloc(sizeof(char)*2);
	strcpy(cargs[0],"clpr");
	strcpy(pargs[0],"clpr");
	strcpy(cargs[1],argc[1]);
	strcpy(pargs[1],argc[1]);
	strcpy(cargs[2],argc[2]);
	strcpy(pargs[2],argc[2]);
	strcpy(cargs[3],"c");
	strcpy(pargs[3],"p");
	cargs[4]=NULL;
	pargs[4]=NULL;

	signal(SIGINT,sig_handle);


	if((key = ftok(argc[1], atoi(argc[2])))==-1){
    	perror("Key error");
		exit(1);
    }
	if((shmid = shmget(key,SHMSIZE,ALL_PERM|IPC_CREAT))==-1){
		perror("Creating shared memory");
		exit(1);
	}
	if((semid = semget(key, NUMSEMS, ALL_PERM | IPC_CREAT )) == -1){
        perror("Creating semaphore");
		exit(1);
    }
    /*        '1' --  The shared memory segment is being used.       */
    /*        '0' --  The shared memory segment is freed.            */
    if(semctl(semid, 0, SETVAL, 0) == -1){
        perror("Creating semaphore");
		exit(1);
    }
    //Initializing shared memory fields
    if((shm_address = (int *)shmat(shmid, NULL, 0))==NULL ){
        perror("Attaching shared memory");
		exit(1);
    }
    shm_address[0]=0;
    shm_address[1]=shm_address[2]=3;


	for(int i=0;i<consnum;i++){
		int pid = fork();
		if(pid==0){
			execv(cargs[0],cargs);
		}
		conpid[i]=pid;
	}
	for(int i=0;i<prodnum;i++){
		pid = fork();
		if(pid==0){
			execv(pargs[0],pargs);
		}
		prodpid[i]=pid;
	}
    while(1);
}