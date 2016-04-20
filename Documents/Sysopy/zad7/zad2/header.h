#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>

const char *spacename = "/space";
const char *semname = "/semaphore";
const char *bsemname = "/binsemaphore";

#ifndef MEMSIZE
	#define MEMSIZE 20000
#endif

#ifndef ARRSIZE
	#define ARRSIZE 1000
#endif