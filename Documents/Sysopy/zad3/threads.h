#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
long long get_real_time(struct timespec * ts);
long long get_system_time(struct rusage * usage);
long long get_user_time(struct rusage * usage);
void set_time(long long * sys_time,long long *us_time,long long *real_time,struct timespec * ts,struct rusage * usage);
void add_to_file(char * type,long long systime,long long utime,long long rtime,int cycles,char * method);
void run_fork(int n);
void run_vfork(int n);
void run_f_clone(int n);
void run_v_clone(int n);
