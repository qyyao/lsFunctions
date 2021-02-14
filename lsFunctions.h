//Annie Yao
//Last modified: February 14th, 2021
//Description: Functions of myls function

#ifndef _LS_FUNCTIONS_H_
#define _LS_FUNCTIONS_H_

#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>

//analyzes if a user option input is a file/directory/invalid
struct optionInfo_s
{
    char * name;
    bool isDirectory;
    bool isValid;
};

//takes file name in a directory and returns a char of new path
char * getFilePath(char * dirPath, char * fileName);

//intakes a file/directory name and checks if it should be hidden due to starting with a dot
bool isDotFile(char * fileName);

//prints the info used in long output
void printLongInformation(struct stat currentFile, int maxLinkColumn, int maxUserIDColumn, int maxGroupIDColumn, int maxSizeColumn);

//returns group name
char * getAndReturnGroup(gid_t grpNum);

//returns username
char * getAndReturnUserName(uid_t uid);

//calculate how many digits are in a long int
int findDigitsNumber(long int x);

//printing folder contents (possibly recursive)
void printDirectory(bool printIndexFormat, bool longListingFormat, bool subdirectoriesFormat, char * directoryName);

void printFile(bool printIndexFormat, bool longListingFormat, char * fileName);

//compares two strings, returns whether the first is alphabetically first
//returns false if strings are equal or the second is alphabetically first 
bool isStringFirst(char * string1, char * string2);

//swaps the location of two indexes in a struct dirent 
void swap(struct optionInfo_s options[], int index1, int index2);

//sorts part of the array entries into alphabetical order
//insertion sort O(n^2)
void sortPartial(struct optionInfo_s options[], int index1, int index2);

//sorts an options array into files section first followed by folders section
int sortOptions(struct optionInfo_s options[], int n);

//swaps the location of two indexes in a struct dirent 
void swapDirent(struct dirent ** array, int index1, int index2);

//sorts the dirent array entries into alphabetical order
//insertion sort O(n^2)
void direntSortAlphabetical(struct dirent ** array, int n);

#endif