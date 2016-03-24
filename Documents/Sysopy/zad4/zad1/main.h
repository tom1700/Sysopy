#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

typedef void sigfunc(int);
sigfunc * signal(int,sigfunc*);
void sig_handler(int);