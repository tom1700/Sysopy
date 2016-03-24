#include "sender.h"

int sigsend;//Signals send
int sigrec;//Signals received
int semaphore;
int sending; //Flag that tells if program is sending or receiving
int cpid;

void send_signals(){
	for(int i=0;i<sigsend;i++){
		//printf("Sender is sending\n");
		semaphore--;
		kill(cpid, SIGUSR1);
		while(semaphore<1){}
	}
	sending = 0;
	kill(cpid, SIGUSR2);
}

void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
    	if(sending==1){
    		semaphore++;
    		//printf("Sender received confirmation\n");
    	}
    	else{
    		//printf("Sender received signal\n");
        	sigrec++;
        	kill(cpid, SIGUSR1);
    	}
    }
    else if(signum == SIGUSR2){
    	printf("Signals send: %d Signals received:%d\n",sigsend,sigrec);
    	exit(0);
    }
}

int main(int argv,char ** argc){
	sigsend = atoi(argc[1]);
	sigrec = 0;
	semaphore = 1;
	sending = 1;

	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);

	char ** cargs = malloc(sizeof(char*)*2);
	cargs[0]="catcher2";
	cargs[1]=NULL;

	cpid = fork();
	if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
      	execv("catcher2", cargs);
    }
    sleep(1);//Giving time for child process to start
    send_signals();
	while(1){}
	return 0;
}