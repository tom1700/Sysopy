#include "server.h"
/*
	Create queue before start (random file without extension)
	and pass it name as first argument and
	some unsigned short as second argument
*/
int csqid;//Needs to be global to be removed after SIGINT
void register_client(int * clients,int * index,mymsg * msg){
	clients[*index]=atoi(msg->mtext);
	sprintf(msg->mtext,"%d",*index);
	msg->mtype = SRV_TO_CL;
	if(msgsnd(clients[*index], msg, MSG_SIZE, IPC_NOWAIT)==-1){
		perror("Sending messege error");
		exit(1);	
	}
	(*index)++;
}
void send_task(int * clients,mymsg * msg){
	int clidindex = atoi(msg->mtext);
	clients[clidindex];
	sprintf(msg->mtext,"%d",rand());
	msg->mtype = SRV_TO_CL;
	if(msgsnd(clients[clidindex], msg, MSG_SIZE, IPC_NOWAIT)==-1){
		perror("Sending messege error");
		exit(1);	
	}
}
void print_result(mymsg * msg){
	char *clid=malloc(4);//No more than 100 so 3 digits are enough
	char *number=malloc(12);//No more than integer range
	int len = strlen(msg->mtext);
	if(msg->mtext[len-1]=='1'){
		int i=0;
		while(msg->mtext[i]!=' '){
			clid[i]=msg->mtext[i];
			i++;
		}
		clid[i]='\0';
		len = strlen(clid);
		i++;
		while(msg->mtext[i]!=' '){
			number[i-len-1]=msg->mtext[i++];
		}
		number[i-len-1]='\0';
		printf("Liczba pierwsza: %s (klient: %s)\n",number,clid);
		free(clid);
		free(number);
	}
}
void sig_handle(int signo){
	if(signo==SIGINT){
		if(msgctl(csqid,IPC_RMID, 0) == -1){
            perror("Closing error");
			exit(1);
        }
		exit(0);
	}
}
int main(int argv,char**argc){
	key_t key;
	mymsg msg;
	int clients[100];
	int index = 0;
	srand(time(NULL));

	signal(SIGINT,sig_handle);

	//client->server queue creation arg[0]-path arg[1]-proj_id
    if((key = ftok(argc[1], atoi(argc[2])))==-1){
    	perror("Key error");
		exit(1);
    }

	if((csqid = msgget(key, ALL_PERM|IPC_CREAT))==-1){
		perror("Creating queue error");
		exit(1);
	}
	
	while(1){
		//Get messege from client
		if(msgrcv(csqid, &msg, MSG_SIZE, 0, MSG_NOERROR)==-1){
			perror("Receiving error");
			exit(1);
		}
		//Client wants to register
		if(msg.mtype==CL_REG){
			if(index<100)
				register_client(clients,&index,&msg);
		}
		//Client ready for task
		else if(msg.mtype==CL_RED){
			send_task(clients,&msg);
		}
		//Client sends result
		else if(msg.mtype==CL_RES){
			print_result(&msg);
		}
	}

	return 0;
}