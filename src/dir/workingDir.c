#include "workingDir.h"
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include "../../include/types.h"

char *working_dir = NULL;

Status status = {0, NULL};

void get_working_dir(){
    char *ptr = NULL;
    int SIZE = 1024;

    while(ptr == NULL){
        char *tmp = realloc(working_dir, SIZE);

        if(tmp == NULL){
            free(working_dir);
            return ;
        }
        working_dir = tmp;

        ptr = getcwd(working_dir, SIZE);

        if(ptr == NULL){
            switch(errno){
                case ERANGE:
                    SIZE *= 2;
                    continue;
                default:
                    free(working_dir);
                    return;
            }
        }
    }
}

Status *set_working_dir(char *path){
    int ret;
    ret = chdir(path);
    printf("return is %i\n", ret);
    if(ret == -1){
        switch (errno){
            case ENOENT:
                status.status = 1;
                status.message = "Specified Path does not exist";
                return &status;
            
            case ENOTDIR:
                status.status = 1;
                status.message = "Path is not a directory";
                return &status;
            
            case EACCES:
                status.status = 1;
                status.message = "Permisssion denied";
                return &status;
            
            default:
                status.status = 1;
                status.message = "Error when setting directory";
                return &status;
        }
    }

    status.message = NULL;
    status.status = 0;

    get_working_dir();

    return &status;
}

