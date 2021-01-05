#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include "lsFunctions.h"

//takes file name in a directory and returns a char of new path
char * getFilePath(char * dirPath, char * fileName){

    char * filePath = malloc(strlen(dirPath) + strlen(fileName) + 2);

    strcpy(filePath, dirPath);
    strcat(filePath, "/");
    strcat(filePath, fileName);

    return filePath;
}

//intakes a file/directory name and checks if it should be hidden due to starting with a dot
bool isDotFile(char * fileName){

    return (fileName[0] == '.');
}

//prints the info used in long output
void printLongInformation(struct stat currentFile, int maxLinkColumn, int maxUserIDColumn, int maxGroupIDColumn, int maxSizeColumn){

    //print permissions (sourced from https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c)
    printf((S_ISDIR(currentFile.st_mode)) ? "d" : "-");
    printf((currentFile.st_mode & S_IRUSR) ? "r" : "-");
    printf((currentFile.st_mode & S_IWUSR) ? "w" : "-");
    printf((currentFile.st_mode & S_IXUSR) ? "x" : "-");
    printf((currentFile.st_mode & S_IRGRP) ? "r" : "-");
    printf((currentFile.st_mode & S_IWGRP) ? "w" : "-");
    printf((currentFile.st_mode & S_IXGRP) ? "x" : "-");
    printf((currentFile.st_mode & S_IROTH) ? "r" : "-");
    printf((currentFile.st_mode & S_IWOTH) ? "w" : "-");
    printf((currentFile.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");

    //print hardlinks #
    printf("%*ld ", maxLinkColumn, currentFile.st_nlink);

    //print user 
    printf("%*s ", maxUserIDColumn, getAndReturnUserName(currentFile.st_uid));

    //print group
    printf("%*s ", maxGroupIDColumn, getAndReturnGroup(currentFile.st_gid));

    //print file size
    printf("%*ld ", maxSizeColumn, currentFile.st_size);

    //print last modified time
    for (int j = 4; j < 11; j++){ //month and date
        printf("%c", ctime(&currentFile.st_mtime)[j]);
    }

    for (int j = 20; j < 24; j++){ //year
        printf("%c", ctime(&currentFile.st_mtime)[j]);
    }            

    for (int j = 10; j < 16; j++){ //time
        printf("%c", ctime(&currentFile.st_mtime)[j]);
    }
}

//returns group name
char * getAndReturnGroup(gid_t grpNum){

    struct group *grp = getgrgid(grpNum);
	return (grp->gr_name);
}

//returns username
char * getAndReturnUserName(uid_t uid){
	
    struct passwd *pw = getpwuid(uid);
	return (pw->pw_name);
}

//calculate how many digits are in a long int
int findDigitsNumber(long int x){

    if (x == 0){
        return 1;
    }

    int digitCount = 0;

    while(x > 0){
        x = x/10;
        digitCount++;
    }

    return digitCount;
}

//printing folder contents (possibly recursive)
void printDirectory(bool printIndexFormat, bool longListingFormat, bool subdirectoriesFormat, char * directoryName){

    struct dirent ** files;
    int fileCount;

    fileCount = scandir(directoryName, &files, NULL, NULL);

    direntSortAlphabetical(files, fileCount);

    struct stat statArray[fileCount];

    //find the max number of digit for each column for padding purposes
    int maxInodeColumn = 0;
    int maxLinkColumn = 0;
    int maxUserIDColumn = 0;
    int maxGroupIDColumn = 0;
    int maxSizeColumn = 0;

    if (longListingFormat || subdirectoriesFormat){

        for (int i = 0; i < fileCount; i++){

	        if (!isDotFile(files[i] -> d_name)){

	            char * filePath = getFilePath(directoryName, files[i] -> d_name);

	            if (stat(filePath, &statArray[i]) == -1){
	                perror("stat");
	            }

	            if (findDigitsNumber(statArray[i].st_ino) > maxInodeColumn){
	                maxInodeColumn = findDigitsNumber(statArray[i].st_ino);
	            }

	            if (findDigitsNumber(statArray[i].st_nlink) > maxLinkColumn){
	                maxLinkColumn = findDigitsNumber(statArray[i].st_nlink);
	            }

	            if (strlen(getAndReturnUserName(statArray[i].st_uid)) > maxUserIDColumn){
	                maxUserIDColumn = strlen(getAndReturnUserName(statArray[i].st_uid));
	            }

	            if (strlen(getAndReturnGroup(statArray[i].st_gid)) > maxGroupIDColumn){
	                maxGroupIDColumn = strlen(getAndReturnGroup(statArray[i].st_gid));
	            }

	            if (findDigitsNumber(statArray[i].st_size) > maxSizeColumn){
	                maxSizeColumn = findDigitsNumber(statArray[i].st_size);
	            }

	            free(filePath);
	        }
        }
    }

        //print each file for i/l
    for (int i = 0; i < fileCount; i++) {

        if (!isDotFile(files[i] -> d_name)){

            //>>>>>>>>>>>OPTION "-i"
            if (printIndexFormat){

                printf("%*ld ", maxInodeColumn, files[i] -> d_ino);
            }

            //>>>>>>>>>>>OPTION "-l"
            if (longListingFormat){

                printLongInformation(statArray[i], maxLinkColumn, maxUserIDColumn, maxGroupIDColumn, maxSizeColumn);

                //print file name
                printf(" %s", files[i] -> d_name);
            }
        
            else{
                
                printf("%s", files[i] -> d_name);
            }

            if (i != fileCount - 1){

                printf("\n");
            }   
        }
    } 

    printf("\n");

    if (subdirectoriesFormat){

        for (int i = 0; i < fileCount; i++){

            //if the file has links and is not a dot file
            if (statArray[i].st_nlink > 1 && (!isDotFile(files[i] -> d_name))){

                //calculate its new path
                char * newDirPath = getFilePath(directoryName, files[i] -> d_name);

                printf("\n");
                printf("%s:\n", newDirPath);
                
                //recursion
                printDirectory(printIndexFormat, longListingFormat, subdirectoriesFormat, newDirPath);

                free(newDirPath);
            }
        }
    }

    for (int i = 0; i < fileCount; i++){
        free(files[i]);
    }

    free(files);
    return;
}

void printFile(bool printIndexFormat, bool longListingFormat, char * fileName){

    struct stat statInfo;

    stat(fileName, &statInfo);

    //>>>>>>>>>>>OPTION "-i"
    if (printIndexFormat){

        printf("%ld ", statInfo.st_ino);
    }

    //>>>>>>>>>>>OPTION "-l"
    if (longListingFormat){

        printLongInformation(statInfo, 0, 0, 0, 0);
    }

    printf("%s\n", fileName);
}

//compares two strings, returns whether the first is alphabetically first
//returns false if strings are equal or the second is alphabetically first 
bool isStringFirst(char * string1, char * string2){

    int i = 0;

    //compare each individual digit of the string until they differ
    while(string1[i] != '\0' && string2[i] != '\0'){

        if (string1[i] < string2[i]){
            return true;
        }

        if (string1[i] > string2[i]){
            return false;
        }

        i++;
    }

    //if the first characters are equal, the one with the shorter string is alphabetically first
    if (strlen(string1) < strlen(string2)){
        return true;
    }

    else {
        return false;
    }
}

//swaps the location of two indexes in a struct dirent 
void swap(struct optionInfo_s options[], int index1, int index2){

    struct optionInfo_s temp;

    temp = options[index1];
    options[index1] = options[index2];
    options[index2] = temp;
}

//sorts part of the array entries into alphabetical order
//insertion sort O(n^2)
void sortPartial(struct optionInfo_s options[], int index1, int index2){

    int current;
    
    for (int i = index1 + 1; i <= index2; i++){

        current = i;

        for (int j = index2; j >= index1; j--){

            if (isStringFirst(basename(options[current].name), basename(options[j].name))){

                swap(options, current, j);
                current = j;
            }
        }
    }
} 

//sorts an options array into files section first followed by folders section
int sortOptions(struct optionInfo_s options[], int n){

    int optionIndex = 0;

    //take all files and put them at the front, and sort it
    for (int i = 0; i < n; i++){

        if (options[i].isValid){

            if (!options[i].isDirectory){

                swap(options, i, optionIndex);
                optionIndex++;
            }
        }
    }

    int fileCount = optionIndex;

    sortPartial(options, 0, optionIndex - 1);

    //take all directories and follow after files, sorted

    for (int i = 0; i < n; i++){

        if (options[i].isValid){

            if (options[i].isDirectory){

                swap(options, i, optionIndex);
                optionIndex++;
            }
        }
    }

    sortPartial(options, fileCount, optionIndex - 1);

    return fileCount;
}

//swaps the location of two indexes in a struct dirent 
void swapDirent(struct dirent ** array, int index1, int index2){

    struct dirent * temp;

    temp = array[index1];
    array[index1] = array[index2];
    array[index2] = temp;
}


//sorts the dirent array entries into alphabetical order
//insertion sort O(n^2)
void direntSortAlphabetical(struct dirent ** array, int n){

    int current;
    
    for (int i = 1; i < n; i++){

        current = i;

        for (int j = i - 1; j >= 0; j--){

            if (isStringFirst(array[current] -> d_name, array[j] -> d_name)){
                swapDirent(array, current, j);
                current = j;
            }
        }
    }
} 