#include "catcher.h"

int sigrec;
int ppid;
union sigval sgv;

void send_signals(){
	for(int i=0;i<sigrec;i++){
		//printf("Catcher is answering\n");
		sigqueue(ppid, SIGRTMIN+1,sgv);

	}
	sigqueue(ppid, SIGRTMIN+2,sgv);
	exit(0);
}

void sig_handler(int signum)
{
    if (signum == SIGRTMIN+1)
    {
       	sigrec++;
    }
    else if(signum == SIGRTMIN+2){
    	send_signals();
    }
}

int main(int argv,char ** argc){
	sigrec = 0;
	ppid = getppid();
	struct sigaction act;
    act.sa_handler = sig_handler;
    sgv.sival_int=0;
    sigaction (SIGRTMIN+1, &act, NULL);
    sigaction (SIGRTMIN+2, &act, NULL);
	while(1){}
	return 0;
}