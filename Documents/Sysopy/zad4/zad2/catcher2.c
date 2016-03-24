#include "catcher.h"

int sigrec;
int ppid;
int semaphore;
int sending; //Flag that tells if program is sending or receiving


void send_signals(){
	for(int i=0;i<sigrec;i++){
		//printf("Catcher is answering\n");
		semaphore--;
		kill(ppid, SIGUSR1);
		while(semaphore<1){}

	}
	kill(ppid, SIGUSR2);
	exit(0);
}

void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
    	if(sending==1){
    		semaphore++;
    		//printf("Catcher received confirmation\n");
    	}
    	else{
    		//printf("Catcher received signal.\n");
        	sigrec++;
        	kill(ppid, SIGUSR1);
    	}
    }
    else if(signum == SIGUSR2){
    	sending = 1;
    	send_signals();
    }
}

int main(int argv,char ** argc){
	sigrec = 0;
	sending = 0;//Program starts with receiving
	semaphore = 1;
	ppid = getppid();
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	while(1){}
	return 0;
}