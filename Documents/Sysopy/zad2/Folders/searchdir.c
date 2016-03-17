#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "searchdir.h"

char * mode_to_str(mode_t mode){
	char * result = malloc(sizeof(char)*11);
	result[0]=(S_ISDIR(mode)) ? 'd' : '-';
    result[1]=(mode & S_IRUSR) ? 'r' : '-';
    result[2]=(mode & S_IWUSR) ? 'w' : '-';
    result[3]=(mode & S_IXUSR) ? 'x' : '-';
    result[4]=(mode & S_IRGRP) ? 'r' : '-';
    result[5]=(mode & S_IWGRP) ? 'w' : '-';
    result[6]=(mode & S_IXGRP) ? 'x' : '-';
    result[7]=(mode & S_IROTH) ? 'r' : '-';
    result[8]=(mode & S_IWOTH) ? 'w' : '-';
    result[9]=(mode & S_IXOTH) ? 'x' : '-';
	result[10]='\0';
	return result;
}

char * add_to_path(char * path, char * name ){
	char * newPath = malloc((strlen(path)+strlen(name)+2)*sizeof(char));
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,name);
	return newPath;
}

char * relative_path(char * startpath, char * path){
	int l1 = strlen(startpath);
	int l2 = strlen(path);
	char * newPath = malloc(sizeof(char)*(l2-l1+1));
	for(int i=l1;i<l2;i++){
		newPath[i-l1] = path[i];
	}
	return newPath;
}
int search_dir(char * path, char * startpath, char * permissions){
	DIR * dirp = opendir(path);
	struct dirent * dp;
	struct stat * filestat = malloc(sizeof(struct stat));
	if(dirp==NULL)
		return 1;
	while ((dp = readdir(dirp)) != NULL) {
		if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0){
			char * direct_path = add_to_path(path,dp->d_name);
			stat(direct_path,filestat);
			//If file is directory get in
			if(S_ISDIR(filestat->st_mode)){
				search_dir(direct_path,startpath,permissions);
			}
		    else if(S_ISREG(filestat->st_mode) && strcmp(permissions,mode_to_str(filestat->st_mode))==0){
		    	char * rel_path = relative_path(startpath,direct_path);
		    	printf("%s %d %d\n",rel_path,filestat->st_size,filestat->st_atime);
				free(rel_path);
			}
			free(direct_path);
		}
	}	
	closedir(dirp);
	return 0;
}

int main(int argc, char **argv){
	char * path = argv[1];
	char * permissions = argv[2];
	if(strlen(path) == 0 || strlen(permissions)==0){
		printf("Bad arguments\n");
		return 1;
	}
	return search_dir(path,path,permissions);
}