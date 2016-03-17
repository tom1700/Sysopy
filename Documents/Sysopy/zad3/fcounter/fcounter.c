#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <syscall.h>

char * add_to_path(char * path, char * name ){
	char * newPath = malloc((strlen(path)+strlen(name)+2)*sizeof(char));
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,name);
	return newPath;
}

int check_ext(char * name,char * ext){
	if(strcmp(ext,"")==0){
		return 1;
	}
	int nl = strlen(name);
	int el = strlen(ext);
	if(el>nl){
		return 0;
	}
	if(name[nl-el-1]!='.')
		return 0;
	for(int i=0;i<el;i++){
		if(name[nl-i-1]!=ext[el-i-1])
			return 0;
	}
	return 1;
}

//varg and warg are flags reffering to -w and -v parameters
int search_dir(char * path,char * ext,int varg, int warg,char ** argv){
	int counter=0;
	int number_of_files;//Neccessary to varg argument
	int wstatus;
	int pstarted=0;
	pid_t cpid, w;
	DIR * dirp = opendir(path);
	struct dirent * dp;
	struct stat * filestat = malloc(sizeof(struct stat));
	if(dirp==NULL)
		perror("directory Error");
	while ((dp = readdir(dirp)) != NULL) {
		if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0){
			
			char * direct_path = add_to_path(path,dp->d_name);
			stat(direct_path,filestat);
			//If file is directory get in
			if(S_ISDIR(filestat->st_mode)){
				setenv("PATH_TO_BROWSE",direct_path,1);
				//Start new process
				cpid = fork();
		        if (cpid == -1) {
		            perror("fork");
		            exit(EXIT_FAILURE);
		        }
		        if (cpid == 0) {
		        	execv("fcounter", argv); 	
		        }
		        pstarted++;
			}
		    if(check_ext(dp->d_name,ext)==1){
		    	counter++;	
			}
			free(direct_path);
		}
	}
	if(warg==1){
		sleep(15);
	}
	number_of_files = counter;
	//Waiting for child processes to finish
	while(pstarted > 0){
		do {
            w = wait(&wstatus);
            if (w == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(wstatus));
        counter += WEXITSTATUS(wstatus);
		pstarted--;
	}
	if(varg==1){
		printf("\nPath: %s\nFiles in directory: %d\nTotal number of files: %d\n",path,number_of_files,counter);
	}
	closedir(dirp);
	return counter;
}

//  Environmental variables to set before start:
//	export PATH_TO_BROWSE='/home/student/Documents/Sysopy/zad3/fcounter'
//	export EXT_TO_BROWSE=''

int main(int argc, char **argv){
	char * path = getenv("PATH_TO_BROWSE");
	char * ext = getenv("EXT_TO_BROWSE");
	int warg=0;
	int varg=0;
	if(path == NULL){
		path = malloc(sizeof(char)*1024);
		getcwd(path, sizeof(path));
	}
	if(ext == NULL){
		ext = malloc(sizeof(char));
		strcpy(ext,"");
	}
	if(argc > 1){
		warg = strcmp(argv[1],"-w") == 0 ? 1 : 0;
		varg = strcmp(argv[1],"-v") == 0 ? 1 : 0;
		if(argc > 2){
			if(strcmp(argv[2],"-w")==0) warg = 1;
			if(strcmp(argv[2],"-v")==0) varg = 1;
		}
	}
	exit(search_dir(path,ext,varg,warg,argv));
}