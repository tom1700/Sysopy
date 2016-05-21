
#include "header.h"

/*
    Arguments:
    arg[1] port
    arg[2] unix socket path
*/
//Necessary global variables
int sock, sock_u, new_sock;

void error(const char *msg){
    
    perror(msg);
    exit(0);

}

void sig_handle(int signum){
    
    if(signum == SIGINT){
        close(sock);
        close(sock_u);
        close(new_sock);
        exit(0);
    }

}

void init_clients_array(client * clients){
    for(int i=0;i<MAXCLAM;i++){
        clients[i].reg_time = -1;
        clients[i].sock = -1;
    }
}
void open_internet_socket(int port, struct sockaddr_in * server, socklen_t * fromlen){
    
    int length;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("Opening socket");
    
    length = sizeof(struct sockaddr_in);
    bzero(server,length);

    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY;
    server->sin_port = htons(port);
    
    if (bind(sock, (struct sockaddr *)server, length) < 0) 
       error("binding");
    (*fromlen) = sizeof(struct sockaddr_in);
    listen(sock,5);
}

void open_unix_socket(char * unix_socket_path,struct sockaddr_un * server_u, socklen_t * fromlen_u){
    
    int length;
    
    if ((sock_u = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
        error("creating socket");
    
    bzero((char *) server_u, sizeof(struct sockaddr_un));
    
    server_u->sun_family = AF_UNIX;
    strcpy(server_u->sun_path, unix_socket_path);
    
    length = strlen(server_u->sun_path) + sizeof(server_u->sun_family);
    
    unlink(unix_socket_path);
    if(bind(sock_u,(struct sockaddr *)server_u,length)<0)
       error("binding socket");
    
    (*fromlen_u) = sizeof(struct sockaddr_un);
    listen(sock_u,5);
}

void set_sockets(fd_set * sock_set,int * max_sock_size,client * clients){
    
    FD_ZERO(sock_set);
    FD_SET(sock, sock_set);
    FD_SET(sock_u, sock_set);
    (*max_sock_size) = (sock > sock_u) ? sock : sock_u;
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].sock != -1){
            FD_SET(clients[i].sock,sock_set);
            (*max_sock_size) = (clients[i].sock > (*max_sock_size)) ? clients[i].sock : (*max_sock_size);
        }
    }

}

int add_internet_client(struct sockaddr_in * from, socklen_t * fromlen, int * clients_amount, client * clients, int * client_end_pos){

    
    new_sock = accept(sock,(struct sockaddr *) from, fromlen);
    
     if((*clients_amount) > MAXCLAM)
        return -1;
    
    time(&clients[(*client_end_pos)].reg_time);
    clients[(*client_end_pos)].sock = new_sock;
    clients[(*client_end_pos)].active = 0;
    clients_amount++;
    
    while(clients[(*client_end_pos)].sock != -1){
        (*client_end_pos) = ((*client_end_pos)+1) % MAXCLAM;
    }
    printf("Adding new client : %d\n",new_sock);

    return 0;
}

int add_unix_client(struct sockaddr_un * from, socklen_t * fromlen, int * clients_amount, client * clients,int * client_end_pos){

    
    new_sock = accept(sock_u,(struct sockaddr *) from, fromlen);
    
    if((*clients_amount) > MAXCLAM)
        return -1;
    
    time(&clients[(*client_end_pos)].reg_time);
    clients[(*client_end_pos)].sock = new_sock;

    clients[(*client_end_pos)].active = 0;
    clients_amount++;
    
    while(clients[(*client_end_pos)].sock != -1){
        (*client_end_pos) = ((*client_end_pos)+1) % MAXCLAM;
    }
    
    printf("Adding new client : %d\n",new_sock);

    return 0;
}

int set_client_active(int socket, client * clients){
    
    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].sock == socket){
            clients[i].active = 1;
            return 0;
        }
    }

    return -1;
}

void read_and_pass_messege(client_messege * buf,int sender_sock, client * clients){

    bzero((char *) buf, MAXMSGSIZE+MAXIDSIZE);
    if ((read(sender_sock,buf,MAXMSGSIZE+MAXIDSIZE)) < 0)
        error("ERROR reading from socket");


    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].sock != -1 && clients[i].sock != sender_sock){
            write(clients[i].sock,buf,MAXMSGSIZE+MAXIDSIZE);
        }
    }

}

void remove_inactive_clients(client * clients, int * clients_amount, fd_set * sock_set){
    
    time_t current_time;
    time(&current_time);

    for(int i=0;i<MAXCLAM;i++){
        if(clients[i].active == 0){
            if(current_time - clients[i].reg_time > 60){
                clients[i].reg_time = -1;
                FD_CLR(clients[i].sock,sock_set);
                close(clients[i].sock);
                clients[i].sock = -1;
                (*clients_amount)--;
            }
        }
    }
}

void run_server(socklen_t * fromlen, socklen_t * fromlen_u){
    
    fd_set sock_set;
    int max_sock_size;
    int clients_amount, client_end_pos;
    struct sockaddr_in from;
    struct sockaddr_un from_u;
    client_messege * buf;
    client clients[MAXCLAM];
    
    clients_amount = 0;
    client_end_pos = 0;
    buf = malloc(sizeof(client_messege));
    
    init_clients_array(clients);

    while(1){
        
        set_sockets(&sock_set, &max_sock_size, clients);
        if(select(max_sock_size + 1, &sock_set, NULL, NULL, NULL) < 0){
            error ("select");
        }

        //remove_inactive_clients(clients, &clients_amount, &sock_set);

        for (int i = 0; i < max_sock_size+1; i++){
            if (FD_ISSET(i, &sock_set)){
                if (i == sock){
                    add_internet_client(&from, fromlen, &clients_amount, clients, &client_end_pos);
                }
                else if(i == sock_u){
                    add_unix_client(&from_u, fromlen_u, &clients_amount, clients, &client_end_pos);
                }
                else{
                    set_client_active(i, clients);
                    read_and_pass_messege(buf, i, clients);
                }
            }
        }
    }
}

int main (int argc, char ** argv){

    int port;
    char * unix_socket_path;
    struct sockaddr_in server;
    struct sockaddr_un server_u;
    socklen_t fromlen, fromlen_u;

    if(argc < 3) perror("Wrong number of arguments.");

    port = atoi(argv[1]);
    unix_socket_path = malloc(strlen(argv[2]));
    strcpy(unix_socket_path,argv[2]);


    open_unix_socket(unix_socket_path, &server_u, &fromlen_u);
    open_internet_socket(port, &server, &fromlen);
    
    signal(SIGINT,sig_handle);

    run_server(&fromlen, &fromlen_u);

    return 0;
}