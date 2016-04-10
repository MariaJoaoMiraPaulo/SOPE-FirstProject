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
#define LINE_SIZE BLANK_SPACE_NAME+BLANK_SPACE_INODE+BLANK_SPACE_SIZE+BLANK_SPACE_DATE+BLANK_SPACE_PERMISSIONS+BLANK_SPACE_DIR

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

void reading_file_to_array(int file_in_order, char *info[], int lines){

/*  int ret;

  int i;
  for(i=0; i< lines; i++){
     ret=read(file_in_order, info, LINE_SIZE);
     if(ret == -1){
       perror("Error reading file");
       exit(1);
     }
    sprintf(info[i], "\n");
   }
  for(i=0; i< lines; i++){
   printf("%s", info[i]);
 }*/
/*
 char* ch;
 FILE *fp;

 fp = fopen("files.txt","r"); // read mode

   if( fp == NULL )
   {
      perror("Error while opening the file.\n");
      exit(1);
   }

   int i=0;
   while( ( (&ch) = fgetc(fp) ) != EOF ){
      sprintf(info[i], ch);
    }

   fclose(fp);*/


}

void check_duplicate_files(int file_in_order, int lines){
//  char buffer[BLANK_SPACE_NAME];
//  int ret= read(file_in_order, buffer, BLANK_SPACE_NAME);
  printf("linhas : %d\n", lines);
//  int i;
//  for(i=0;i<lines;i++){

  //}
}

int main(int argc, char	*argv[]) {

  if	(argc != 2) {
    fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
    exit(1);
  }

  int file_in_order=open("files.txt", O_WRONLY |  O_APPEND | O_CREAT | O_RDONLY , 0600);

  if(file_in_order == -1){
    perror("Error opening the file files.txt");
    exit(1);
  }

  pid_t pid=fork();
  int status;
//  char *info[MAX_LENGTH];

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
    else if ( pid > 0){
      wait(&status);
      //int lines=countlines("files.txt");
      //reading_file_to_array(file_in_order, info, lines);
      //check_duplicate_files(file_in_order, lines);
    }
    else if ( pid == 0){
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
