#include "server.h"

int scqid;//Needs to be global to be closed by SIGINT handler

void send_ready_signal(int csqid,mymsg * msg,int clid){
	msg->mtype = CL_RED;
	sprintf(msg->mtext,"%d",clid);
	if(msgsnd(csqid, msg, MSG_SIZE, IPC_NOWAIT)==-1){
		perror("Sending messege error");
		exit(1);	
	}
}
int wait_for_data(mymsg * msg){
	if(msgrcv(scqid, msg, MSG_SIZE, 0, MSG_NOERROR)==-1){
		perror("Receiving error");
		exit(1);
	}
	return atoi(msg->mtext);
}
int isprime(int num){
	if (num < 2) return 0;
	if (num==2) return 1;
    if (num % 2 == 0) return 0;
    for(int i = 3; i < num / 2; i+= 2)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}
void send_result(int csqid,mymsg * msg){
	msg->mtype = CL_RES;
	printf("Sending result %s\n",msg->mtext);
	if(msgsnd(csqid, msg, MSG_SIZE, IPC_NOWAIT)==-1){
		perror("Sending messege error");
		exit(1);	
	}
}
void sig_handle(int signo){
	if(signo==SIGINT){
		if(msgctl(scqid,IPC_RMID, 0) == -1){
            perror("Closing error");
			exit(1);
        }
		exit(0);
	}
}
int main(int argv,char**argc){
	int csqid;
	mymsg msg;
	int clid;
	int tasknum;

	signal(SIGINT,sig_handle);

	//client->server queue opening arg[0]-path arg[1]-proj_id
    key_t key = ftok(argc[1], atoi(argc[2]));

	if((csqid = msgget(key, ALL_PERM))==-1){
		perror("Opening queue error");
		exit(1);
	}

	//server->client queue creating
	if((scqid = msgget(IPC_PRIVATE, ALL_PERM))==-1){
		perror("Creating queue error");
		exit(1);
	}
	printf("%d\n",scqid);
	//client->server  registration
	msg.mtype = CL_REG;
	sprintf(msg.mtext,"%d",scqid);
	if(msgsnd(csqid, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
		perror("Sending messege error");
		exit(1);	
	}
	//server->client gettin id
	if(msgrcv(scqid, &msg, MSG_SIZE, 0, MSG_NOERROR)==-1){
		perror("Receiving error");
		exit(1);
	}
	clid = atoi(msg.mtext);

	while(1){
		//Client ready for task
		send_ready_signal(csqid,&msg,clid);
		//Client wait for number from server
		tasknum = wait_for_data(&msg);
		//Check if prime and respond
		sprintf(msg.mtext,"%d %d %d",clid,tasknum,isprime(tasknum));
		send_result(csqid,&msg);
	}
	return 0;
}