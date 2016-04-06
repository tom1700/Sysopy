#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef MAXLINE
	#define MAXLINE 20
#endif