#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include "searchdir2.h"

char * permissions;

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
int handle_file(const char *fpath, const struct stat *sb,int typeflag){
	if(typeflag != FTW_F)
		return 0;
	char * file_perm = mode_to_str(sb->st_mode);
	if(strcmp(file_perm,permissions)==0){
		printf("%s %d %d\n",fpath,sb->st_size,sb->st_atime);
	}
	free(file_perm);
	return 0;
}
int main(int argc, char **argv){
	char * path = argv[1];
	permissions = argv[2];
	if(strlen(path) == 0 || strlen(permissions)==0){
		printf("Bad arguments\n");
		return 1;
	}
	ftw(path,handle_file,50);
	return 0;
}