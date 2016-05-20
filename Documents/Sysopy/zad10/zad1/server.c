
#include "header.h"

/*
    Arguments:
    arg[1] port
    arg[2] unix socket path
*/
client clients[MAXCLAM];
int clients_amount = 0;
int client_end_pos = 0;
int sock, sock_u;
socklen_t fromlen, fromlen_u;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
//Try to register new client
int add_internet_client(char * id, struct sockaddr_in from){
    if(clients_amount > MAXCLAM)
        return -1;
    clients[client_end_pos].id = malloc(strlen(id)*sizeof(char));
    strcpy(clients[client_end_pos].id,id);
    time(&clients[client_end_pos].reg_time);
    clients[client_end_pos].from = from;
    clients[client_end_pos].type = 0;
    clients_amount++;
    //find next free slot
    while(clients[client_end_pos].id != NULL){
        client_end_pos = (client_end_pos+1) % MAXCLAM;
    }
    return 0;
}

//If clients is registered extend his registration time. Else try to add him
int update_internet_client(char *id, struct sockaddr_in from){
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].id != NULL){
            if(strcmp(id,clients[i].id)==0){
                time(&clients[i].reg_time);
                return 0;
            }
        }
    }
    return add_internet_client(id,from);
}
//Try to register new client
int add_local_client(char * id, struct sockaddr_un from){
    if(clients_amount > MAXCLAM)
        return -1;
    clients[client_end_pos].id = malloc(strlen(id)*sizeof(char));
    strcpy(clients[client_end_pos].id,id);
    time(&clients[client_end_pos].reg_time);
    clients[client_end_pos].from_u = from;
    clients[client_end_pos].type=1;
    clients_amount++;
    //find next free slot
    while(clients[client_end_pos].id != NULL){
        client_end_pos = (client_end_pos+1) % MAXCLAM;
    }
    return 0;
}

//If clients is registered extend his registration time. Else try to add him
int update_local_client(char *id, struct sockaddr_un from){
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].id != NULL){
            if(strcmp(id,clients[i].id)==0){
                time(&clients[i].reg_time);
                return 0;
            }
        }
    }
    return add_local_client(id,from);
}

void remove_inactive_clients(){
    time_t current_time;
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].id!=NULL){
            time(&current_time);
            if(current_time - clients[i].reg_time > 60){
                printf("%s kicked from conversation\n",clients[i].id);
                free(clients[i].id);
                clients[i].id = NULL;
                clients[i].reg_time = -1;
                clients_amount--;
            }
        }
    }
}
void pass_messege(client_messege * msg){
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].id != NULL){
            if(strcmp(clients[i].id,msg->id)!=0){
                printf("Sending to %s\n",clients[i].id);
                if(clients[i].type == 0){
                    int n = sendto(sock,msg,MAXMSGSIZE+MAXIDSIZE,0,(const struct sockaddr *)&clients[i].from,fromlen);
                    if (n < 0)
                        error("sendto");
                }
                else if(clients[i].type == 1){
                    int n = sendto(sock_u,msg,MAXMSGSIZE+MAXIDSIZE,0,(const struct sockaddr *)&clients[i].from_u,fromlen_u);
                    if (n < 0)
                        error("sendto");    
                }
            }
        }
    }
}
void sig_handle(int signum){
    if(signum == SIGINT){
        exit(0);
    }
}

int main(int argc, char **argv)
{
    fd_set rfds;
	int length, length_u, n;
	struct sockaddr_in from, server;
    struct sockaddr_un from_u, server_u;
	client_messege * buf = malloc(sizeof(client_messege));

    if (argc < 3) {
      fprintf(stderr, "Not enough arguments.\n");
      exit(0);
    }   

    //----------------------------------------Internet Socket----------------------------------------------------------------------
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Opening socket");
	length = sizeof(server);
	bzero(&server,length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));//Converts address to network byte order
	if (bind(sock, (struct sockaddr *)&server, length) < 0) 
	   error("binding");
	fromlen = sizeof(struct sockaddr_in);
	//---------------------------------------Unix socket-----------------------------------------------------------------------------
    unlink(argv[2]);
    if ((sock_u = socket(AF_UNIX,SOCK_DGRAM,0)) < 0)
        error("creating socket");
    bzero((char *) &server_u, sizeof(server_u));
    server_u.sun_family = AF_UNIX;
    strcpy(server_u.sun_path, argv[2]);
    length_u = strlen(server_u.sun_path) + sizeof(server_u.sun_family);
    if(bind(sock_u,(struct sockaddr *)&server_u,length_u)<0)
       error("binding socket");
    fromlen_u = sizeof(struct sockaddr_un);
    //---------------------------------------------Data init-------------------------------------
    //Init clients array
    for(int i=0;i<MAXCLAM;i++){
        clients[i].id = NULL;
        clients[i].reg_time = -1;
    }

    signal(SIGINT,sig_handle);
    //-------------------------------------------------main loop-------------------------------------------------------
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        FD_SET(sock_u, &rfds);
        select(((sock > sock_u) ? sock : sock_u)+1, &rfds, NULL, NULL, NULL);
        if(FD_ISSET(sock, &rfds)){
            n = recvfrom(sock, buf, MAXMSGSIZE+MAXIDSIZE, 0, (struct sockaddr *)&from, &fromlen);
            if (n < 0)
                error("recvfrom");
            remove_inactive_clients();
            printf("Received messege from: %s\n",buf->id);
            if(update_internet_client(buf->id,from)==-1){
                n = sendto(sock,"Server is full\n",17,0,(struct sockaddr *)&from,fromlen);
                if (n  < 0)
                    error("sendto");
            }
            else{
                pass_messege(buf);
            }
        }
        bzero((char *) buf, MAXMSGSIZE+MAXIDSIZE);
        if(FD_ISSET(sock_u,&rfds)){
            n = recvfrom(sock_u, buf, MAXMSGSIZE+MAXIDSIZE, 0, (struct sockaddr *)&from_u, &fromlen_u);
            if (n < 0)
                error("recvfrom");
           
            remove_inactive_clients();
            printf("Received messege from: %s\n",buf->id);
            
            if(update_local_client(buf->id,from_u) == -1){
                n = sendto(sock_u,"Server is full\n", 17, 0, (struct sockaddr *)&from_u, fromlen_u);
                if (n < 0)
                    error("sendto");
            }
            else{
                pass_messege(buf);
            }
        }
        bzero((char *) buf, MAXMSGSIZE+MAXIDSIZE);
	}
	return 0;
 }
