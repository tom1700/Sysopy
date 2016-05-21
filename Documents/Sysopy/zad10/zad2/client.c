/* UDP client in the internet domain */
/*
    Arguments:
    arg[1] user id(string)
    arg[2] server type, local: l or internet: i
    arg[3] if i: address IP else unix socket path
    arg[4] if i server port,

    example ./client bartek i 192.168.133.132 2000
*/
#include "header.h"


client_messege * tosend;
client_messege * toreceive;
pthread_t iothreadid;
pthread_t mainthreadid;
int sock;
unsigned int length;
struct sockaddr_in server;
struct sockaddr_un server_u;
struct hostent *hp;
char * inputbuf;
char type[2];
int n;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}
void sig_handle(int signum){
    if(signum==SIGUSR1){
        if (write(sock, tosend, MAXMSGSIZE+MAXIDSIZE) < 0)
            error("Sendto");
    }
    else if(signum==SIGINT){
        pthread_cancel(iothreadid);
        close(sock);
        exit(0);
    }
}
void * iothread(void * args){
    inputbuf = malloc((MAXMSGSIZE-1)*sizeof(char));
    while(1){
        bzero(inputbuf,MAXMSGSIZE);
        fgets(inputbuf,MAXMSGSIZE-1,stdin);
        strtok(inputbuf, "\n");
        strcpy(tosend->messege,inputbuf);
        pthread_kill(mainthreadid,SIGUSR1);
    }    
}

void loop(){
    while(1){
        bzero(toreceive->messege,MAXMSGSIZE);
        bzero(toreceive->id,MAXIDSIZE);
        if (read(sock,toreceive,MAXMSGSIZE+MAXIDSIZE) < 0)
            error("recvfrom");
        printf("%s: %s\n",toreceive->id,toreceive->messege);
    }
}

int main(int argc, char **argv)
{
    tosend = malloc(sizeof(client_messege));
    toreceive = malloc(sizeof(client_messege));

    if (argc < 4) { 
		error("Wrong number of parameters");
	}
	strcpy(tosend->id,argv[1]);
    strcpy(type,argv[2]);
	//--------------------------------------Opening socket-----------------------------------------------------------------------
    //internet socket
    if(type[0]=='i'){
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    	error("Opening socket");
	    server.sin_family = AF_INET;
	    if ((hp = gethostbyname(argv[3])) == 0)
	        error("Unknown host");
	    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	    server.sin_port = htons(atoi(argv[4]));
	    length = sizeof(struct sockaddr_in);
	    if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) 
	        error("ERROR connecting");
    }
    /*Unix sockets. Here comes magic*/
    else if(type[0]=='l'){   	
	    bzero((char *)&server_u,sizeof(server_u));
	    server_u.sun_family = AF_UNIX;
	    strcpy(server_u.sun_path, argv[3]);
	    length = strlen(server_u.sun_path) + sizeof(server_u.sun_family);
	    if ((sock = socket(AF_UNIX, SOCK_STREAM,0)) < 0)
	       error("Creating socket");
	    if (connect(sock, (struct sockaddr *)&server_u, length) < 0)
	        error("Connecting");
    }
    //---------------------------------------------------Starting thread, signal handling--------------------------------------------------------------
    mainthreadid = pthread_self();
    if(pthread_create(&iothreadid, NULL, iothread, NULL)==-1){
        perror("Thread");
        exit(1);
    }
    signal(SIGUSR1,sig_handle);
    //----------------------------------------------------Client-server thread loop----------------------------------------------------------------
    loop();
	return 0;
}