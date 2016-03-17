#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

char * mode_to_str(mode_t mode);
char * add_to_path(char * path, char * name );
char * relative_path(char * startpath, char * path);
int search_dir(char * path, char * startpath, char * permissions);