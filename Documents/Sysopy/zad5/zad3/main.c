#include "main.h"

int main(int argv,char** argc){
	char * output = argc[1];
	FILE *pipein_fp, *pipeout_fp;
    char readbuf[80];
	pipein_fp = popen("ls -l", "r"); 		//opens pipe to the ls command
	freopen(output, "w", stdout);
	pipeout_fp = popen("grep ^d", "w");	//opens pipe to the sort command

    /* Processing loop */
    while(fgets(readbuf, 80, pipein_fp)){
        fputs(readbuf, pipeout_fp);
    }
    /* Close the pipes */
    pclose(pipein_fp);
    pclose(pipeout_fp);
    return 0;
}