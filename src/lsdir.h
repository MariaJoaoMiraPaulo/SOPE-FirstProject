#ifndef LSDIR_H
#define LSDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>

/**
* Write regular files information on file given by file_d
*/
void update_file(int file_d, struct stat *stat_buf, struct	dirent *direntp, char dir[]);




#endif
