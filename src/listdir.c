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

#define MAX_LENGTH 100
#define BLANK_SPACE 20

void fill_with_blank_spaces(char buffer[], int length){
	int i;
	for(i=0; i<length; i++){
		strcat(buffer, " ");
	}
}

void preparing_info(char buffer[], char second_buffer[]){
	int space;

	space = BLANK_SPACE - strlen(second_buffer);
	fill_with_blank_spaces(second_buffer, space);
	strcat(buffer,second_buffer);

	strcpy(second_buffer, "");
}

void update_file(int file_d, struct stat *stat_buf, struct	dirent *direntp){
	char buffer[MAX_LENGTH];
	char second_buffer[MAX_LENGTH];

	strcpy(second_buffer, "");   //cleaning string, to avoid errors
	strcpy(buffer, "");

	strcpy(second_buffer, direntp->d_name);
	preparing_info(buffer, second_buffer);

	sprintf(second_buffer, "%" PRIuMAX, (uintmax_t)direntp->d_ino);
	preparing_info(buffer,second_buffer);

	sprintf(second_buffer, "%jd", (intmax_t)stat_buf->st_size);
	preparing_info(buffer,second_buffer);

	struct tm * timeinfo;
	timeinfo = localtime (&(stat_buf->st_mtime));
	strftime(second_buffer, 20, "%b %d %H:%M", timeinfo);
	preparing_info(buffer,second_buffer);

	sprintf(second_buffer, "%o", stat_buf->st_mode);
	preparing_info(buffer,second_buffer);

	strcat(buffer, "\n");
	write(file_d, buffer, strlen(buffer));
}

pid_t create_process(char arg[]){
	pid_t pid;
	pid=fork();

	if(pid < 0){
		perror("fork ERROR");
		exit(3);
	}

	else if(pid == 0){
		execlp("listdir","listdir", arg, NULL);
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
	int file_d=open("file_disorderly.txt", O_WRONLY |  O_APPEND | O_CREAT | O_RDONLY , 0600);

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
				update_file(file_d, &stat_buf, direntp);
			}
			else  if (S_ISDIR(stat_buf.st_mode)){
				str = "directory";
				if(strcmp( direntp->d_name, ".")  && strcmp( direntp->d_name, ".."))
				create_process(direntp->d_name);
			}
			else str = "other";
			if(strcmp( direntp->d_name, ".")  && strcmp( direntp->d_name, ".."))
			printf("%-25s - %s\n", direntp->d_name, str);
		}
		closedir(dirp);
		close(file_d);

		exit(0);
	}
