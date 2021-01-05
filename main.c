// Name: main.c
// Author: Annie Yao
// Date: Aug 3rd, 2020
// Function: imitates the ls function in unix with options -i, -l and -R

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include "lsFunctions.h"
#define _GNU_SOURCE

int main(int argCount, char ** argv){

    //used to keep track of which of the three formats we need
    bool printIndexFormat = false;
    bool longListingFormat = false;
    bool subdirectoriesFormat = false;

    //tracks if user inputted file arguments
    bool printOwnDirectory = true;

    //tracks argument index 
    int argIndex;

    //analyze options inputted in arguments
    for(argIndex = 1; argIndex < argCount; argIndex++){

        int j;

        //if it is a dash argument, analyze which of the three it fits
        if (argv[argIndex][0] == '-'){
            
            j = 1;

            while (argv[argIndex][j] != '\0'){
                
                if (argv[argIndex][j] == 'i'){
                    printIndexFormat = true;
                }

                if (argv[argIndex][j] == 'l'){
                    longListingFormat = true;
                }

                if (argv[argIndex][j] == 'R'){
                    subdirectoriesFormat = true;
                }         
                j++;       
            }
        }

        //exit loop when encountering first file name
        else{
            printOwnDirectory = false;
            break;
        }
    }

    //analyse user inputs:
    int optionCount = argCount - argIndex;

    if (optionCount == 0){

        optionCount = 1;
        printOwnDirectory = true;
    }

    struct optionInfo_s options[optionCount];

    //create of list of files/directories

    if (printOwnDirectory){

        options[0].name = ".";
        options[0].isValid = true;
        options[0].isDirectory = true;
    }

    else{
        //Populate array of options
        for (int i = 0; i < optionCount; i++){

            options[i].name = argv[argIndex];
        
            //first check if it is a directory 
            struct dirent ** files;
            int fileCount;

            fileCount = scandir(options[i].name, &files, NULL, NULL);
            
            if (fileCount > -1){
                
                options[i].isDirectory = true;
                options[i].isValid = true;

                for (int j = 0; j < fileCount; j++){

                    free(files[j]);
                }

                free(files);
            }

            //if not directory, check if it is a file

            else{

                struct stat fileStat;
                
                if (stat(argv[argIndex], &fileStat) == -1){

                    options[i].isValid = false;
                    options[i].isDirectory = false;
                    printf("myls: cannot access %s: No such file or directory\n", argv[argIndex]);
                }

                else{
                    options[i].isValid = true;
                    options[i].isDirectory = false;
                }
            } 

            argIndex++;  
        } 
    }

    int fileCount = sortOptions(options, optionCount);

    //print each file
    for (int i = 0; i < fileCount; i++){

        printFile(printIndexFormat, longListingFormat, options[i].name);                 
    }

    if (fileCount != 0){

        if (!longListingFormat || optionCount > fileCount){
            printf("\n");
        }
    }

    //print each folder
    for (int i = fileCount; i < optionCount; i++){

        if (options[i].isValid){
            
            if (optionCount > 1 || subdirectoriesFormat){

                printf("%s:\n", options[i].name);
            }

            printDirectory(printIndexFormat, longListingFormat, subdirectoriesFormat, options[i].name);
        }  

        if (options[i + 1].isValid && i != optionCount - 1){
            printf("\n");
        }
    }
}