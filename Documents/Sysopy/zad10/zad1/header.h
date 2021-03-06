#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#ifndef MAXMSGSIZE
	#define MAXMSGSIZE 256
#endif

#ifndef MAXIDSIZE
	#define MAXIDSIZE 256
#endif

#ifndef MAXCLAM
	#define MAXCLAM 48
#endif

typedef struct client{
	time_t reg_time;
	char * id;
	struct sockaddr_in from;
	struct sockaddr_un from_u;
	int type; //0-internet, 1-local
}client;

typedef struct client_messege{
	char id[MAXIDSIZE];
	char messege[MAXMSGSIZE];
}client_messege;

