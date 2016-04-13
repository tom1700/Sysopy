#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#ifndef CL_REG
	#define CL_REG 1 //Client want to register
#endif

#ifndef CL_RED
	#define CL_RED 2 //Client ready for task
#endif

#ifndef CL_RES
	#define CL_RES 3 //Client sends result
#endif

#ifndef SRV_TO_CL
	#define SRV_TO_CL 4 //Client sends result
#endif

#ifndef CL_END
	#define CL_END 5 //Clients informs server about exit
#endif

typedef struct mymsg mymsg;
struct mymsg {
	long int type;
	int clid;
	int num;
	char scname[11];//Used to send queue name or flag if number is prime
};