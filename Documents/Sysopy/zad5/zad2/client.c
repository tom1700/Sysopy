#include "myheader.h"

void createMessege(char * buf, char * input){
	time_t writetime;
	time(&writetime);
	int pid = getpid();
	char time_buff[16];
	strftime(time_buff, 15, "%H:%M:%S", localtime(&writetime));
    sprintf(buf, "%d %s %s", pid, time_buff,input);
}

void createEndMessege(char * buf){
	time_t writetime;
	time(&writetime);
	int pid = getpid();
	char time_buff[16];
	strftime(time_buff, 15, "%H:%M:%S", localtime(&writetime));
    sprintf(buf, "%d %s end", pid, time_buff);
}

int main(int argv,char**argc)
{
	char messege[BUFF_SIZE];
    int fd;
    char * fifoname = argc[1];
    char buf[21];
    if((fd = open(fifoname, O_WRONLY|O_NONBLOCK))<0){
		perror("open error");
		exit(1);
	}
	while(1){
    	printf("Enter a messege. Max 20 signs. Write q to end.\n");
    	fgets(buf,20,stdin);
    	buf[strlen(buf)-1]=0;
    	if(strcmp("q",buf)==0){
    		createEndMessege(messege);
    		write(fd, messege, strlen(messege)+1);
    		break;
    	}
    	createMessege(messege,buf);
    	write(fd, messege, strlen(messege)+1);
    	memset(messege,0,BUFF_SIZE);
    	memset(buf,0,21);
	}
	close(fd);
    return 0;
}