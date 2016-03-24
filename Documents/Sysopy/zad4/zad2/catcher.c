#include "catcher.h"

int sigrec;
int ppid;


void send_signals(){
	for(int i=0;i<sigrec;i++){
		//printf("Catcher is answering\n");
		kill(ppid, SIGUSR1);

	}
	kill(ppid, SIGUSR2);
	exit(0);
}

void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
       	sigrec++;
    }
    else if(signum == SIGUSR2){
    	send_signals();
    }
}

int main(int argv,char ** argc){
	sigrec = 0;
	ppid = getppid();
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	while(1){}
	return 0;
}