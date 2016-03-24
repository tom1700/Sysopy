#include "sender.h"

int sigsend;//Signals send
int sigrec;//Signals received
int cpid;

void send_signals(){
	for(int i=0;i<sigsend;i++){
		//printf("Sender is sending\n");
		kill(cpid, SIGUSR1);
	}
	kill(cpid, SIGUSR2);
}

void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
   		//printf("Sender received signal\n");
       	sigrec++;
    }
    else if(signum == SIGUSR2){
    	printf("Signals send: %d Signals received:%d\n",sigsend,sigrec);
    	exit(0);
    }
}

int main(int argv,char ** argc){
	sigsend = atoi(argc[1]);
	sigrec = 0;

	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);

	char ** cargs = malloc(sizeof(char*)*2);
	cargs[0]="catcher";
	cargs[1]=NULL;

	cpid = fork();
	if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
      	execv("catcher", cargs);
    }
    sleep(1);
    send_signals();
	while(1){}
	return 0;
}