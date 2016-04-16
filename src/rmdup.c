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

#define LINE_SIZE_ON_FILE 146
#define INDEX_NAME 0
#define INDEX_INODE 21
#define INDEX_SIZE 42
#define INDEX_DATE 63
#define INDEX_PERMISSION 84
#define INDEX_PATH 105
#define SIZE_BUFFER_NAME 200
#define SIZE_BUFFER_PATH 200
#define MAX_NUMBER_FILES 100

typedef struct {
  char name[SIZE_BUFFER_NAME];
  unsigned int size;
  char path[SIZE_BUFFER_PATH];
}Compare_files;

void reseting_files(){
  FILE* file1 = fopen("files.txt", "w");
  FILE* file2 = fopen("file_disorderly.txt" , "w");

  if( file1 == NULL || file2 == NULL){
    perror("Error on opening files to compare content" );
    exit(1);
  }

  fclose(file1);
  fclose(file2);
}

//Count the number of lines in the file called filename
int countlines(char *filename)
{
  //Open the file
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL)
  return 0;

  //Check how many '\n' (lines) the file have, reading char by char
  while ((ch = fgetc(fp)) != EOF)
  {
    if (ch == '\n')
    lines++;
  }
  fclose(fp);
  return lines;
}

//Read file and load information on array of the type Compare_files (struct)
void reading_file_to_array(Compare_files info[], int lines){

  int i=0;
  int ret;
  //Open file
  FILE* file_in_order = fopen("files.txt", "r");
  char buffer[LINE_SIZE_ON_FILE];
  char *second_buffer[6];

  if (file_in_order == NULL){
    perror("Error on opening file");
    exit(1);
  }

  while(i < lines){
    //Read an entire line with the information of one regular file
    ret=fread(buffer, sizeof(char), LINE_SIZE_ON_FILE, file_in_order);
    if(ret != LINE_SIZE_ON_FILE){
      perror("Error on reading the file");
      exit(1);
    }

    //Add an '\0' on the space between two strings, to indicate the end of the string and the beggining of another
    buffer[INDEX_INODE-1]='\0';
    buffer[INDEX_SIZE-1]='\0';
    buffer[INDEX_DATE-1]='\0';
    buffer[INDEX_PERMISSION-1]='\0';
    buffer[INDEX_PATH-1]='\0';
    buffer[LINE_SIZE_ON_FILE-1]='\0';

    second_buffer[0]=&buffer[INDEX_NAME];  //Name
    second_buffer[1]=&buffer[INDEX_INODE]; //Inode
    second_buffer[2]=&buffer[INDEX_SIZE];  //Size
    second_buffer[3]=&buffer[INDEX_DATE];  //Date
    second_buffer[4]=&buffer[INDEX_PERMISSION];  //Permissions
    second_buffer[5]=&buffer[INDEX_PATH];    //Path

    //Load information to the struct info
    strcpy(info[i].name,second_buffer[0]);
    info[i].size = atoi(second_buffer[2]);
    strcpy(info[i].path,second_buffer[5]);

    i++;
  }


  fclose(file_in_order);
}


int compare_time_last_data_modification( char* path_file_1, char* path_file_2){

  path_file_1 = strtok(path_file_1, " ");
  path_file_2 = strtok(path_file_2, " ");

  struct stat buf;
  time_t path_file_1_time,path_file_2_time;
  double seconds;

  //Fill struct stat of file_1 to get the time of last data modification.
  lstat(path_file_1,&buf);
  path_file_1_time=buf.st_mtime;

  //Fill struct stat of file_2 to get the time of last data modification.
  lstat(path_file_2,&buf);
  path_file_2_time=buf.st_mtime;

  //Calculate the diff between the two dates
  seconds=difftime(path_file_1_time,path_file_2_time);

  //if the diff between the two dates are bigger than 0 it means that file1 date is bigger->1_file is more recent
  if(seconds>0){
    printf("Ficheiro %s é mais recente que ficheiro %s \n",path_file_1,path_file_2);
    return 1;

  }

  //if the diff between the two dates are lesser than 0 it means that file1 date is lesser-> 1_file is older
  else if(seconds<0){
    printf("ficheiro %s é mais antigo que  %s \n",path_file_1,path_file_2);
    return 2;
  }

  else{
    printf("as datas sao iguais\n");
    return 0;
  }
  return 0;

}

int compare_file_permissons(char *path_file_1, char *path_file_2){

  path_file_1 = strtok(path_file_1, " ");
  path_file_2 = strtok(path_file_2, " ");

  struct stat buf;
  int path_file_1_permissons,path_file_2_permissons;

  //Fill struct stat of file_1 to get the permissons of the file.
  lstat(path_file_1,&buf);
  path_file_1_permissons=buf.st_mode;

  //Fill struct stat of file_2 to get the permissons of the file.
  lstat(path_file_2,&buf);
  path_file_2_permissons=buf.st_mode;

  //the two files have the same permissons
  if(path_file_1_permissons==path_file_2_permissons){
    return 0;
  }


  return 1;

}

int compare_file_content(char *path_file_1, char *path_file_2){

  path_file_1 = strtok(path_file_1, " ");
  path_file_2 = strtok(path_file_2, " ");

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

int files_equals_to(Compare_files info[],int position, int* index, int size_of_array){

  int i,ret=0,j=1;
  bool found=false;
  for(i=position+1;i<size_of_array;i++){
    if(info[position].size == info[i].size){
      if(compare_file_permissons(info[position].path, info[position].path)==0){
        if(strcmp(info[position].name,info[i].name)==0){
          if(compare_file_content(info[position].path, info[position].path)==0){
            if(!found){
              index[j]=position;
              j++;
              ret++;
              found=true;
            }
            ret++;
            index[j]=i;
            j++;
          }
        }
      }
    }
  }

  if(found)
    index[0]=ret+1;    //size of each line of array index

  return ret;

}

void creating_hard_links(Compare_files info[], int info_size, int index[MAX_NUMBER_FILES][MAX_NUMBER_FILES], int index_size){

  int ret_date;
  int more_recent_file = 1;

  int i;
  for(i=0; i<index_size; i++){
    int j;
    printf("Tamanho do index : %d", index[i][0]);
    for(j = 2; j<index[i][0]; j++){
      ret_date = compare_time_last_data_modification(info[index[i][more_recent_file]].path, info[index[i][j]].path);
      if(ret_date >= 0){
        more_recent_file = j-1;
      }
      else more_recent_file = j;
    }
    for(j = 1; j<index[i][0]; j++){
      if( j != more_recent_file){
        printf("Vou fazer um link\n");
        unlink(info[index[i][j]].path);
        link(info[index[i][more_recent_file]].path, info[index[i][j]].path);
      }
    }
  }

}

void check_duplicate_files(Compare_files info[], int size_of_array){
  int i,ret=0,x=0;
  int index[MAX_NUMBER_FILES][MAX_NUMBER_FILES];
  bool found=false;

  //int i;
  for(i=0; i< size_of_array; i++){
    ret=files_equals_to(info,i,index[x],size_of_array);
    if(ret>0){
      found=true;
      x++;
      i=i+ret-1;
    }

  }

  if(found){
    creating_hard_links(info, size_of_array, index, x);
  }

  for(i=0;i<x;i++){
    printf("NUMERO DE FICHEIROS IGUAIS %d \n",index[i][0]-1);
  }

}

int main(int argc, char	*argv[]) {


  if	(argc != 2) {
    fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
    exit(1);
  }

  reseting_files();

  int file_in_order=open("files.txt",   O_APPEND | O_CREAT | O_WRONLY , 0600);

  if(file_in_order == -1){
    perror("Error opening the file files.txt");
    exit(1);
  }

  pid_t pid=fork();
  int status;

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
    execlp("./listdir","listdir", argv[1], NULL);
    perror("execlp ERROR");
    exit(1);
  }

  close(file_in_order);

  exit(0);
}
