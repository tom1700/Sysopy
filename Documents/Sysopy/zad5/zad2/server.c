#include "myheader.h"

int checkIfEnd(char * messege){
	int len = strlen(messege);
	if(messege[len-1]=='d' && messege[len-2]=='n' && messege[len-3]=='e')
		return 1;
	return 0;
}
void printMessege(char * messege){
	time_t readtime;
	time(&readtime);
	char time_buff[16];
	strftime(time_buff, 15, "%H:%M:%S", localtime(&readtime));
	printf("%s %s\n",time_buff,messege);
}
int main(int argv,char**argc){
	char * fifoname = argc[1];
	int fd;
	char messege[BUFF_SIZE];

	unlink(fifoname);

	if((mkfifo(fifoname, S_IRWXU))<0){
		perror("mkfifo error");
		exit(1);
	}
	if((fd = open(fifoname, O_RDONLY))<0){
		perror("open error");
		exit(1);
	}
	while(1){
		read(fd, messege, BUFF_SIZE);
		printMessege(messege);
		if(checkIfEnd(messege)){
			break;
		}
	}
    close(fd);
    unlink(fifoname);

    return 0;
}