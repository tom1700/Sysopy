#include "sender.h"

int sigsend;//Signals send
int sigrec;//Signals received
int cpid;
union sigval sgv;

void send_signals(){
	for(int i=0;i<sigsend;i++){
		//printf("Sender is sending\n");
		sigqueue(cpid, SIGRTMIN+1,sgv);
	}
	sigqueue(cpid, SIGRTMIN+2,sgv);
}

void sig_handler(int signum)
{
    if (signum == SIGRTMIN+1)
    {
   		//printf("Sender received signal\n");
       	sigrec++;
    }
    else if(signum == SIGRTMIN+2){
    	printf("Signals send: %d Signals received:%d\n",sigsend,sigrec);
    	exit(0);
    }
}

int main(int argv,char ** argc){
	sigsend = atoi(argc[1]);
	sigrec = 0;
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigaction (SIGRTMIN+1, &act, NULL);
    sigaction (SIGRTMIN+2, &act, NULL);
    sgv.sival_int=0;

	char ** cargs = malloc(sizeof(char*)*2);
	cargs[0]="catcher3";
	cargs[1]=NULL;

	cpid = fork();
	if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
      	execv("catcher3", cargs);
    }
    sleep(1);
    send_signals();
	while(1){}
	return 0;
}