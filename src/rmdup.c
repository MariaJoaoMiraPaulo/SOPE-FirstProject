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

int main(int argc, char	*argv[]) {

  if	(argc != 2) {
    fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
    exit(1);
  }

  //int file_d=open("file_disorderly.txt", O_WRONLY |  O_APPEND | O_CREAT | O_RDONLY , 0600);
  int file_in_order=open("files.txt", O_WRONLY |  O_APPEND | O_CREAT | O_RDONLY , 0600);

  if(file_in_order == -1){
    perror("Error opening the file files.txt");
    exit(1);
  }

  pid_t pid=fork();
  int status;

  //dup2(file_d, STDOUT_FILENO);

  if(pid == -1){
    perror("Error on fork");
    exit(1);
  }
  else if(pid > 0){   //father
    wait(&status);
    dup2(file_in_order, STDOUT_FILENO);
    execlp("sort", "sort", "file_disorderly.txt", NULL);
    perror("execlp ERROR");
    exit(1);
  }
  else if(pid == 0){  //child
    execlp("listdir","listdir", argv[1], NULL);
    perror("execlp ERROR");
    exit(1);
  }

  close(file_in_order);

  exit(0);
}
