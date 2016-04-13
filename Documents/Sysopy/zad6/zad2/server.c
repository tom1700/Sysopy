#include "server.h"
/*
	Create queue before start (random file without extension)
	and pass it name as first argument and
	some unsigned short as second argument
*/
mqd_t csqid;//Needs to be global to be removed after SIGINT
char * csname;
mymsg msg;

void register_client(mqd_t * clients,int * index){
	if((clients[*index] = mq_open(msg.scname, O_RDWR))==-1){
		perror("Opening queue error");
		exit(1);
	}
	msg.clid = *index;
	msg.type = SRV_TO_CL;
	if(mq_send(clients[*index], &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
	(*index)++;
}
void unregister_client(mqd_t * clients){
	if(mq_close(clients[msg.clid])==-1){
		perror("closing error");
		exit(1);
	}
	clients[msg.clid]=-1;
}

void send_task(int * clients){
	msg.num=rand();
	msg.type = SRV_TO_CL;
	if(mq_send(clients[msg.clid], &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
}

void print_result(){
	if(msg.scname[0]=='1'){
		printf("Liczba pierwsza: %d (klient: %d)\n",msg.num,msg.clid);
	}
}

void sig_handle(int signo){
	if(signo==SIGINT){
		if(mq_close(csqid)==-1){
			perror("closing error");
			exit(1);
		}
		if((mq_unlink(csname))==-1){
			perror("unlinking error");
			exit(1);
		}
		exit(0);
	}
}
int main(int argv,char**argc){
	/*
	key_t key;
	srand(time(NULL));
	*/
	int index = 0;
	mqd_t clients[100];
	csname = argc[1];
	
	signal(SIGINT,sig_handle);

	//Remove queue if exists
	mq_unlink(argc[1]);
	
	//client->server queue creation arg[0]-path arg[1]-proj_id
	struct mq_attr attr;  
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = sizeof(mymsg);  
	attr.mq_curmsgs = 0;  
	if((csqid = mq_open(argc[1],O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,&attr))==-1){
		perror("Creating queue error");
		exit(1);
	}

	while(1){
		//Get messege from client
		if(mq_receive(csqid, &msg,sizeof(mymsg),NULL)==-1){
			perror("Receiving error");
			exit(1);
		}
		//Client wants to register
		if(msg.type==CL_REG){
			if(index<100)
				register_client(clients,&index);
		}
		//Client ready for task
		else if(msg.type==CL_RED){
			send_task(clients);
		}
		//Client sends result
		else if(msg.type==CL_RES){
			print_result();
		}
		//Client quits
		else if(msg.type==CL_END){
			unregister_client(clients);
		}
	}
	return 0;
}