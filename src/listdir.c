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

#define MAX_LENGTH 300
#define INODE_LENGTH 50
#define DATE_LENGTH 50

void update_file(int file_d, struct stat *stat_buf, struct	dirent *direntp, char dir[]){

	char buffer[MAX_LENGTH];
	char temp_date[DATE_LENGTH];
	char temp_inode[INODE_LENGTH];

	sprintf(temp_inode, "%" PRIuMAX, (uintmax_t)direntp->d_ino);


	struct tm * timeinfo;
	timeinfo = localtime (&(stat_buf->st_mtime));
	strftime(temp_date, 20, "%b %d %H:%M", timeinfo);


	sprintf(buffer,"%-20s %-20s %-20jd %-20s %-20o %-40s\n",direntp->d_name,temp_inode,(intmax_t)stat_buf->st_size,temp_date,stat_buf->st_mode,dir);
	write(file_d,buffer,strlen(buffer));


}

pid_t create_process(char arg[]){
	pid_t pid;
	pid=fork();

	if(pid < 0){
		perror("fork ERROR");
		exit(3);
	}

	else if(pid == 0){
		execlp("./listdir","listdir", arg, NULL);
		perror("execLp ERROR");
		exit(1);
	}

	wait(NULL);
	return pid;
}

int main(int argc, char	*argv[]) {
	DIR *dirp;
	struct	dirent *direntp;
	struct	stat stat_buf;
	char	*str;
	char	name[200];
	int file_d=open("file_disorderly.txt", O_WRONLY |  O_APPEND | O_CREAT  , 0600);

	if(file_d == -1){
		perror("Error opening the file file_disorderly.txt");
		exit(1);
	}

	if	(argc != 2) {
		fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
		exit(1);
	}
	if	((dirp = opendir( argv[1])) == NULL) {
		perror(argv[1]);
		exit(2);
	}
	while	((direntp = readdir( dirp)) != NULL) {
		sprintf(name,"%s/%s",argv[1],direntp->d_name);
		// alternativa a chdir(); ex: anterior
		if (  lstat (name, &stat_buf)==-1) {
			perror("lstat ERROR");
			exit(3);                                     }
			//      printf("%10d - ",(int) stat_buf.st_ino);
			if(S_ISREG(stat_buf.st_mode)) {
				str = "regular";
				update_file(file_d, &stat_buf, direntp, name);
			}
			else  if (S_ISDIR(stat_buf.st_mode)){
				str = "directory";
				if(strcmp( direntp->d_name, ".")  && strcmp( direntp->d_name, ".."))
				create_process(name);
			}
			else str = "other";
			if(strcmp( direntp->d_name, ".")  && strcmp( direntp->d_name, ".."))
			printf("%-25s - %s\n", direntp->d_name, str);
		}
		closedir(dirp);
		close(file_d);

		exit(0);
	}
