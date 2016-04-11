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

#define MAX_LENGTH 1000
#define BLANK_SPACE_NAME 20
#define BLANK_SPACE_INODE 10
#define BLANK_SPACE_SIZE 8
#define BLANK_SPACE_DATE 15
#define BLANK_SPACE_PERMISSIONS 8
#define BLANK_SPACE_DIR 40
#define LINE_SIZE BLANK_SPACE_NAME+BLANK_SPACE_INODE+BLANK_SPACE_SIZE+BLANK_SPACE_DATE+BLANK_SPACE_PERMISSIONS+BLANK_SPACE_DIR+2 //+2 because of the \n

int countlines(char *filename)
{
  // count the number of lines in the file called filename
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL)
  return 0;

  while ((ch = fgetc(fp)) != EOF)
  {
    if (ch == '\n')
    lines++;
  }
  fclose(fp);
  return lines;
}

void reading_file_to_array(char *info[]){

  int i=0;
  FILE* file_in_order = fopen("files.txt", "r");

  if (file_in_order == 0){
    perror("Error on opening file");
    exit(1);
  }

  info[i] = malloc(LINE_SIZE);
  while(fgets(info[i], LINE_SIZE, file_in_order)){
    printf("%s", info[i]);
    i++;
    info[i] = malloc(LINE_SIZE);
  }

  fclose(file_in_order);

}

void check_duplicate_files(char *info[], int lines){

  char buffer_name1[BLANK_SPACE_NAME];
  char buffer_name2[BLANK_SPACE_NAME];

  int i;
  for(i=0;i<lines; i++){
    memcpy(buffer_name1, info[i], BLANK_SPACE_NAME);
    int j;
    for(j=0;j<lines; j++){
      if(j!=i){
        memcpy(buffer_name2, info[j], BLANK_SPACE_NAME);
        printf("info[i]: %s%d\n", buffer_name1,i);
        printf("info[j]: %s%d\n", buffer_name2,j);
        if(strcmp(buffer_name1, buffer_name2) == 0){
          printf("São iguais\n");
        }
      }
    }
  }

}

int main(int argc, char	*argv[]) {

  if	(argc != 2) {
    fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
    exit(1);
  }

  int file_in_order=open("files.txt",   O_APPEND | O_CREAT | O_WRONLY , 0600);

  if(file_in_order == -1){
    perror("Error opening the file files.txt");
    exit(1);
  }

  pid_t pid=fork();
  int status;
  char *info[LINE_SIZE];

  //dup2(file_d, STDOUT_FILENO);

  if(pid == -1){
    perror("Error on fork");
    exit(1);
  }
  else if(pid > 0){   //father
    wait(&status);
    pid = fork();
    if ( pid == -1){
      perror("Error on fork");
      exit(1);
    }
    else if ( pid > 0){   //father
      wait(&status);
      int lines=countlines("files.txt");
      reading_file_to_array( info);
      check_duplicate_files( info, lines);
    }
    else if ( pid == 0){   //child
      dup2(file_in_order, STDOUT_FILENO);
      execlp("sort", "sort", "file_disorderly.txt", NULL);
      perror("execlp ERROR");
      exit(1);
    }
  }
  else if(pid == 0){  //child
    execlp("listdir","listdir", argv[1], NULL);
    perror("execlp ERROR");
    exit(1);
  }

  close(file_in_order);

  exit(0);
}
