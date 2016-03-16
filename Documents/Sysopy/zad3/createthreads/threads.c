#include "threads.h"

long long counter;

void set_time(long long * sys_time, long long * us_time,long long * real_time, struct timespec * ts, struct rusage * usage){
	*sys_time = get_system_time(usage);
	*us_time = get_real_time(ts);
	*real_time = get_user_time(usage);
}

long long get_real_time(struct timespec * ts){
	clock_gettime(CLOCK_REALTIME, ts);
	return (long long)ts->tv_sec*1000 + ts->tv_nsec/1000000;
}
long long get_system_time(struct rusage * usage){
	getrusage(RUSAGE_SELF,usage);
	return usage->ru_stime.tv_sec*1000+usage->ru_stime.tv_usec/1000;
}
long long get_user_time(struct rusage * usage){
	getrusage(RUSAGE_SELF,usage);
	return usage->ru_utime.tv_sec*1000+usage->ru_utime.tv_usec/1000;
}
void add_to_file(char * type,long long systime,long long utime,long long rtime,int cycles,char * method){
	FILE * handle = fopen("times.txt", "a");
	fprintf(handle,"thread:%s %s iterations: %d\nSystem time:%lld\nReal time:%lld\nUser time:%lld\n",type,method,cycles,systime,utime,rtime);
	fclose(handle);
}
void run_fork(int n){
	for(int i=0;i<n;i++){
		pid_t cpid, w;
        int wstatus;
        cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid == 0) {            /* Code executed by child */
          	counter++;
            _exit(1);
        } else {                    /* Code executed by parent */
            do {
                w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
                if (w == -1) {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
            } while (!WIFEXITED(wstatus));
        }
	}
}
void run_vfork(int n){
	pid_t cpid, w;
    int wstatus;
	for(int i=0;i<n;i++){
        cpid = vfork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (cpid == 0) {            /* Code executed by child */
          	counter++;
            _exit(1);
        } else {                    /* Code executed by parent */
            do {
                w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
                if (w == -1) {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
            } while (!WIFEXITED(wstatus));
        }
	}
}
void run_f_clone(int n){
	char * stack = malloc(STACK_SIZE);
	char * stack_top = stack + STACK_SIZE;
	pid_t cpid, w;
    int wstatus;
	for(int i=0;i<n;i++){
		cpid = clone(child_func, stack_top, SIGCHLD, NULL);
    	if (cpid == -1){
        	perror("clone");
        	exit(EXIT_FAILURE);
        }
        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(wstatus));
	}
}
void run_v_clone(int n){
	char * stack = malloc(STACK_SIZE);
	char * stack_top = stack + STACK_SIZE;
	pid_t cpid, w;
    int wstatus;
	for(int i=0;i<n;i++){
		cpid = clone(child_func, stack_top, CLONE_VM | CLONE_VFORK | SIGCHLD, NULL);
    	if (cpid == -1){
        	perror("clone");
        	exit(EXIT_FAILURE);
        }
        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(wstatus));
	}
}
int child_func(void * arg){
	counter++;
	return 0;
}
int main(int argc,char ** argv){
	int n = atoi(argv[1]);
	char * function = argv[2];
	struct rusage * usage = malloc(sizeof(struct rusage));
	struct timespec * ts = malloc(sizeof(struct timespec));
	long long ptsystem;
	long long ptuser;
	long long ptreal;
	long long ctsystem;
	long long ctuser;
	long long ctreal;
	long long ptsystem2;
	long long ptuser2;
	long long ptreal2;
	long long ctsystem2;
	long long ctuser2;
	long long ctreal2;
	
	set_time(&ptsystem,&ptuser,&ptreal,ts,usage);
	counter=0;

	if(strcmp("fork",function)==0){
		set_time(&ctsystem,&ctuser,&ctreal,ts,usage);
		run_fork(n);
		set_time(&ctsystem2,&ctuser2,&ctreal2,ts,usage);
		set_time(&ptsystem2,&ptuser2,&ptreal2,ts,usage);
		add_to_file("child",ctsystem2-ctsystem,ctuser2-ctuser,ctreal2-ctreal,n,function);
		add_to_file("parent",ptsystem2-ptsystem,ptuser2-ptuser,ptreal2-ptreal,n,function);
		printf("Counter: %lld\n",counter);
		return 0;
	}
	if(strcmp("vfork",function)==0){
		set_time(&ctsystem,&ctuser,&ctreal,ts,usage);
		run_vfork(n);
		set_time(&ctsystem2,&ctuser2,&ctreal2,ts,usage);
		set_time(&ptsystem2,&ptuser2,&ptreal2,ts,usage);
		add_to_file("child",ctsystem2-ctsystem,ctuser2-ctuser,ctreal2-ctreal,n,function);
		add_to_file("parent",ptsystem2-ptsystem,ptuser2-ptuser,ptreal2-ptreal,n,function);
		printf("Counter: %lld\n",counter);
		return 0;
	}
	if(strcmp("f_clone",function)==0){
		set_time(&ctsystem,&ctuser,&ctreal,ts,usage);
		run_f_clone(n);
		set_time(&ctsystem2,&ctuser2,&ctreal2,ts,usage);
		set_time(&ptsystem2,&ptuser2,&ptreal2,ts,usage);
		add_to_file("child",ctsystem2-ctsystem,ctuser2-ctuser,ctreal2-ctreal,n,function);
		add_to_file("parent",ptsystem2-ptsystem,ptuser2-ptuser,ptreal2-ptreal,n,function);
		printf("Counter: %lld\n",counter);
		return 0;
	}
	if(strcmp("v_clone",function)==0){
		set_time(&ctsystem,&ctuser,&ctreal,ts,usage);
		run_v_clone(n);
		set_time(&ctsystem2,&ctuser2,&ctreal2,ts,usage);
		set_time(&ptsystem2,&ptuser2,&ptreal2,ts,usage);
		add_to_file("child",ctsystem2-ctsystem,ctuser2-ctuser,ctreal2-ctreal,n,function);
		add_to_file("parent",ptsystem2-ptsystem,ptuser2-ptuser,ptreal2-ptreal,n,function);
		printf("Counter: %lld\n",counter);
		return 0;
	}
	return 1;
}