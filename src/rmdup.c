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
#include <stdbool.h>

#define MAX_LENGTH 1000
#define BLANK_SPACE_NAME 20
#define BLANK_SPACE_INODE 10
#define BLANK_SPACE_SIZE 8
#define BLANK_SPACE_DATE 15
#define BLANK_SPACE_PERMISSIONS 8
#define BLANK_SPACE_DIR 40
#define LINE_SIZE BLANK_SPACE_NAME+BLANK_SPACE_INODE+BLANK_SPACE_SIZE+BLANK_SPACE_DATE+BLANK_SPACE_PERMISSIONS+BLANK_SPACE_DIR+2 //+2 because of the \n

typedef struct {
  int day;
  int month;
  int hour;
  int min;
} Date;

typedef struct {
  char name[BLANK_SPACE_NAME+1];
  unsigned int size;
  char path[BLANK_SPACE_DIR+1];
  Date modification_date;
}Compare_files;

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

void reading_file_to_array(Compare_files info[], int lines){

  int i=0;
  int ret;
  FILE* file_in_order = fopen("files.txt", "r");
  char buffer[LINE_SIZE];
  char buffer_size[BLANK_SPACE_SIZE+1];

  if (file_in_order == NULL){
    perror("Error on opening file");
    exit(1);
  }
  /*
  info[i] = malloc(LINE_SIZE);
  while(fgets(info[i], LINE_SIZE, file_in_order)){
  printf("%s", info[i]);
  i++;
  info[i] = malloc(LINE_SIZE);
}
*/

while(i < lines){
  ret=fread(buffer,(BLANK_SPACE_NAME+BLANK_SPACE_INODE), sizeof(char),file_in_order);
  if(ret != sizeof(char)){
    perror("Error on reading the file");
    exit(1);
  }
  memcpy(info[i].name, buffer, BLANK_SPACE_NAME);
  info[i].name[BLANK_SPACE_NAME]='\0';

  ret=fread(buffer, BLANK_SPACE_SIZE, sizeof(char), file_in_order);
  if(ret != sizeof(char)){
    perror("Error on reading the file");
    exit(1);
  }
  memcpy(buffer_size, buffer, BLANK_SPACE_SIZE);
  buffer_size[BLANK_SPACE_SIZE]='\0';
  info[i].size = atoi(buffer_size);

  ret=fread(buffer, BLANK_SPACE_DATE+BLANK_SPACE_PERMISSIONS, sizeof(char), file_in_order);
  if(ret != sizeof(char)){
    perror("Error on reading the file");
    exit(1);
  }

  ret=fread(buffer, BLANK_SPACE_DIR, sizeof(char), file_in_order);
  if(ret != sizeof(char)){
    perror("Error on reading the file");
    exit(1);
  }

  memcpy(info[i].path, buffer, BLANK_SPACE_DIR+1);
  info[i].path[BLANK_SPACE_DIR]='\0';

  ret=fread(buffer, 1 , sizeof(char), file_in_order);
  if(ret != sizeof(char)){
    perror("Error on reading the file");
    exit(1);
  }

  i++;
}

fclose(file_in_order);

}

int compare_file_content(char *path_file_1, char *path_file_2){
  FILE* file_1= fopen(path_file_1, "r");
  FILE* file_2= fopen(path_file_2, "r");
  bool eof=false;
  int ch_file_1;
  int ch_file_2;

  if(file_1 == NULL || file_2 == NULL){
    perror("Error on opening files to compare content" );
    exit(1);
  }

  while(!eof){
    ch_file_1=getc(file_1);
    ch_file_2=getc(file_2);

    if(ch_file_1 != ch_file_2){
      return 1;
    }

    if(ch_file_1 == EOF || ch_file_2 == EOF){
      eof=true;
    }
  }

  return 0;
}

void check_duplicate_files(Compare_files info[], int size_of_array){

  int i;
  for(i=0; i< size_of_array; i++){
    int j;
    for(j=i+1; j< size_of_array; j++){
      if(strcmp(info[i].name, info[j].name) == 0 && info[i].size==info[j].size){
        if(compare_file_content(info[i].path, info[j].path)==0){
          i++;
          printf("Os dois ficheiros sao iguais\n");
        }
      }
      if(info[i].name[0] != info[j].name[0]){
        break;
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
      Compare_files info[lines];
      reading_file_to_array(info, lines);
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
