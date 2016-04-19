#include <sys/msg.h>
#include <sys/shm.h> 
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#ifndef ALL_PERM
	#define ALL_PERM 0600 //Read by user.
#endif
#ifndef SHMSIZE
	#define SHMSIZE 4012
#endif
#ifndef SHMARRSIZE
	#define SHMARRSIZE 1000
#endif
#ifndef NUMSEMS
	#define NUMSEMS 4000
#endif
