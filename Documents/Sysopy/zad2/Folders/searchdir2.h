#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
char * mode_to_str(mode_t mode);
int handle_file(const char *fpath, const struct stat *sb,int typeflag);