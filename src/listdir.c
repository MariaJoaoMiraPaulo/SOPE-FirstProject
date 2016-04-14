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

//Write regular files information on file given by file_d
void update_file(int file_d, struct stat *stat_buf, struct	dirent *direntp, char dir[]){

	char buffer[MAX_LENGTH];
	char temp_date[DATE_LENGTH];
	char temp_inode[INODE_LENGTH];

	sprintf(temp_inode, "%" PRIuMAX, (uintmax_t)direntp->d_ino); //load inode information

	//load date of last file change information
	struct tm * timeinfo;
	timeinfo = localtime (&(stat_buf->st_mtime));
	strftime(temp_date, 20, "%b %d %H:%M", timeinfo);

	//Buffer now have all information about regular file (name, inode,size, date of last change,permissons and path)
	sprintf(buffer,"%-20s %-20s %-20jd %-20s %-20o %-40s\n",direntp->d_name,temp_inode,(intmax_t)stat_buf->st_size,temp_date,stat_buf->st_mode,dir);

	//Write buffer on file
	write(file_d,buffer,strlen(buffer));
}

pid_t create_process(char arg[]){
	pid_t pid;
	pid=fork();

	//Error on fork
	if(pid < 0){
		perror("fork ERROR");
		exit(3);
	}

	//Child
	else if(pid == 0){
		//executes listdir for another directory
		execlp("./listdir","listdir", arg, NULL);
		perror("execLp ERROR");
		exit(1);
	}

	wait(NULL); //Wait for any child to terminate
	return pid;
}

int main(int argc, char	*argv[]) {
	DIR *dirp;
	struct	dirent *direntp;
	struct	stat stat_buf;
	char	*str;
	char	name[200];

	//Opening a file named file_disorderly only for writing. If the file does not exist, it will be created.
	//The file is opened in append mode, before each write the offset is positioned ate the enf of file
	int file_d=open("file_disorderly.txt", O_WRONLY |  O_APPEND | O_CREAT  , 0600);

	if(file_d == -1){
		perror("Error opening the file file_disorderly.txt");
		exit(1);
	}

	//Missing arguments (instruction directory)
	if	(argc != 2) {
		fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
		exit(1);
	}

	//Error opening the directory given by argument
	if	((dirp = opendir( argv[1])) == NULL) {
		perror(argv[1]);
		exit(2);
	}

	//check the directory given by argument
	while	((direntp = readdir( dirp)) != NULL) {
		sprintf(name,"%s/%s",argv[1],direntp->d_name);
		// alternativa a chdir(); ex: anterior
		if (  lstat (name, &stat_buf)==-1) {
			perror("lstat ERROR");
			exit(3);                                     }

			//If it is a regular file, write information on file
			if(S_ISREG(stat_buf.st_mode)) {
				str = "regular";
				update_file(file_d, &stat_buf, direntp, name);
			}//If it is a directory create process
			else  if (S_ISDIR(stat_buf.st_mode)){
				str = "directory";
					//If directory name is "." or ".." it should be ignored
				if(strcmp( direntp->d_name, ".")!=0  && strcmp( direntp->d_name, "..")!=0)
				create_process(name);
			}
			else str = "other";

			//If directory name is "." or ".." it should be ignored
			if(strcmp( direntp->d_name, ".")!=0  && strcmp( direntp->d_name, "..")!=0)
			printf("%-25s - %s\n", direntp->d_name, str);
		}
		closedir(dirp);
		close(file_d);

		exit(0);
	}
