#include "server.h"

mqd_t csqid;
mqd_t scqid;
char scname[11];
mymsg msg;

void send_ready_signal(){
	msg.type = CL_RED;
	if(mq_send(csqid, &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
}

int wait_for_data(){
	if(mq_receive(scqid, &msg,sizeof(mymsg),NULL)==-1){
		perror("Receiving error");
		exit(1);
	}
	return msg.num;
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

void send_result(){
	msg.type = CL_RES;
	printf("Sending result %d %s\n",msg.num,msg.scname);
	if(mq_send(csqid, &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
}

void send_end_signal(){
	msg.type = CL_END;
	if(mq_send(csqid, &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
}
void sig_handle(int signo){
	send_end_signal();
	if(signo==SIGINT){
		if(mq_close(scqid)==-1){
			perror("closing error");
			exit(1);
		}
		if(mq_close(csqid)==-1){
			perror("closing error");
			exit(1);
		}
		if((mq_unlink(scname))==-1){
			perror("unlinking error");
			exit(1);
		}
		exit(0);
	}
}
void random_name(char * name){
	name[0]='/';
	int i;
	for(i=1;i<10;i++){
		name[i]=rand()%('z'-'a')+'a';
	}
	name[i]='\0';
}
int main(int argv,char**argc){
	int tasknum;
	srand(time(NULL));
	signal(SIGINT,sig_handle);
	//client->server queue opening arg[0]-path arg[1]-proj_id
	if((csqid = mq_open(argc[1], O_RDWR))==-1){
		perror("Opening queue error");
		exit(1);
	}
	//server->client queue creating
	random_name(scname);
	struct mq_attr attr;  
	attr.mq_flags = 0;  
	attr.mq_maxmsg = 10;  
	attr.mq_msgsize = sizeof(mymsg);  
	attr.mq_curmsgs = 0;
	if((scqid = mq_open(scname,O_RDWR |O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,&attr))==-1){
		perror("Creating queue error");
		exit(1);
	}
	//client->server  registration
	msg.type = CL_REG;
	strcpy(msg.scname,scname);
	if(mq_send(csqid, &msg, sizeof(mymsg),1)){
		perror("Sending messege error");
		exit(1);	
	}
	//server->client gettin id
	if(mq_receive(scqid, &msg,sizeof(mymsg),NULL)==-1){
		perror("Receiving error");
		exit(1);
	}
	while(1){
		//Client ready for task
		send_ready_signal();
		//Client wait for number from server
		tasknum = wait_for_data();
		//Check if prime and respond
		sprintf(msg.scname,"%d",isprime(tasknum));
		send_result();
	}
	return 0;
}