#include "main.h"

FILE * handle;
char * word;
int recn;
sem_t bsem;
pthread_t * threads;
int tham;

int search(FILE * handle,int amount,char * word){
	record * buff = malloc(amount*RECSIZE);
	sem_wait(&bsem);
	int read = fread(buff, RECSIZE, amount, handle);
	sem_post(&bsem);
	if(read==0)
		return -2;
	for(int i=0;i<read;i++){
		if(strcmp(buff[i].text,word)==0)
			return buff[i].id;
	}
	free(buff);
	return -1;

}
void * thread(void * args){
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
	int id;
	while((id = search(handle,recn,word)) == -1){}
	if(id != -2){
		for(int i=0;i<tham;i++){
			if(threads[i]!=pthread_self())
				pthread_cancel(threads[i]);
		}
		printf("TID:%ld RID:%d\n",pthread_self(),id);
	}
	pthread_exit(0);
}
int main(int argv, char ** argc){
	tham = atoi(argc[1]);
	char * filename = argc[2];
	recn = atoi(argc[3]);
	word = argc[4];
	threads = malloc(sizeof(pthread_t)*tham);
	if(sem_init(&bsem, 0, 1)==-1){
		printf("Semaphore creation");
		exit(1);
	}
	if((handle = fopen(filename,"rb"))==NULL){
		perror("File open");
		exit(1);
	}
	for(int i=0;i<tham;i++){
		if(pthread_create(&threads[i], NULL, thread, NULL)==-1){
			perror("Thread");
			exit(1);
		}
	}
	for(int i=0;i<tham;i++){
		pthread_join(threads[i], NULL);
	}
	return 0;
}