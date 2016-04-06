#include "main.h"

int	main(int argv,char**argc){
	char * fold = argc[1];
	char line[MAXLINE];
	FILE * fpin;
	int	fd[2];
	pid_t childpid;

	//Setting child arguments
	char ** cargs = malloc(sizeof(char*)*3);
	cargs[0] = malloc(sizeof(char)*7);
	strcpy(cargs[0],"./ltou");
	cargs[1] = malloc(sizeof(char)*10);
	strcpy(cargs[1],argc[1]);
	cargs[2]=NULL;

	//Stream direction child---->parent
    pipe(fd);
    childpid = fork();
    if(childpid == 0)
    {
    	//Child closes output of stream
    	close(fd[0]);
		if (fd[1] != STDOUT_FILENO) {
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
		}
        execv("./ltou",cargs);
    }
    //Parent closes input of stream
    close(fd[1]);
    if ( (fpin = fdopen(fd[0], "r")) == NULL){
    	perror("fdopen error"); 
    	exit(1);
    }

	while(1){
		fflush(stdout);
		if (fgets(line, MAXLINE, fpin) == NULL) /* czytamy z łącza */
			break;
		if (fputs(line, stdout) == EOF){
			perror("fputs error to pipe"); 
    		exit(1);
		}
	}
	close(fd[1]);
	if (waitpid(childpid, NULL, 0) < 0){
		perror("waitpid error"); 
    	exit(1);
	}
	putchar('\n');
	return 0;
}