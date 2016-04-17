#ifndef RMDUP_H
#define RMDUP_H

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

#define SIZE_BUFFER_NAME 200
#define SIZE_BUFFER_PATH 200
#define MAX_NUMBER_FILES 100

typedef struct {
  char name[SIZE_BUFFER_NAME];
  unsigned int size;
  char path[SIZE_BUFFER_PATH];
  unsigned int inode;
}Compare_files;

/**
* erase content of files.txt and file_disorderly.txt
*/
void reseting_files();

/**
* @brief Count the number of lines in the file called filename
*
* @ret number of lines of a file
*/
int countlines(char *filename);

/**
* @brief Read file and load information on array of the type Compare_files (struct)
*/
void reading_file_to_array(Compare_files info[], int lines);

/**
* @brief Compare time of last data modification of two files, one with path_file_1 and another with path_file_2
*
* @ret return 0 if they have equal dates, return 1 if file_1 is more recent than file_2, and return 2 if
*  file_1 is older than file_2
*/
int compare_time_last_data_modification( char* path_file_1, char* path_file_2);

/**
* @brief Compare files's modes, one with path_file_1 and another with path_file_2
*
* @ret return 0 if they have equals permissons , 1 if they don't
*/
int compare_file_permissons(char *path_file_1, char *path_file_2);

/**
* @brief Compare file content of two files,one with path_file_1 and another with path_file_2
*
* @ret return 0 if they have equal content , 1 if they don't
*/
int compare_file_content(char *path_file_1, char *path_file_2);

/**
* @brief Return how many files are equal to file loaded on info[position]
*
* @ret return the number of elements of each line of the array index
*/
int files_equals_to(Compare_files info[],int position, int* index, int size_of_array);

/**
* @brief This function creates the hard links and writes to the file hlinks.txt
*/
void creating_hard_links(Compare_files info[], int info_size, int index[MAX_NUMBER_FILES][MAX_NUMBER_FILES], int index_size);

/**
* Search for files that are equal
*/
void check_duplicate_files(Compare_files info[], int size_of_array);

/**
* Do the fork to sort the file
*/
void fork_to_sort_file(int file_in_order);




#endif
