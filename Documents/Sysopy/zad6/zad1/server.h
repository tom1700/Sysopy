#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifndef ALL_PERM
	#define ALL_PERM 0600 //Read by user.
#endif

#ifndef MSG_SIZE
	#define MSG_SIZE 512
#endif

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

typedef struct mymsg mymsg;
struct mymsg {
	long mtype;
	char mtext[MSG_SIZE];
};