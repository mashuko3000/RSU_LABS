#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>

#define ER_INV_PARM 1
#define ER_OPEN_FILE -1
#define OK 0
#define ER_PID 4

int searching_string_int_file(const char* path, const char* str)
{
    FILE* fp;
    ssize_t read;
    size_t len = 0;
    int count;
    char* buf = NULL;

    if (str == NULL || path == NULL) return ER_INV_PARM;

    fp = fopen(path, "r");
    if (fp == NULL) return ER_OPEN_FILE;

    while((read = getline(&buf, &len, fp)) != -1)
    {
        if(strstr(buf, str) != NULL)
        {
            count++;
        }
    }

    fclose(fp);
    if(buf){
        free(buf);
    }

    return OK;
}

void fork_bomb(int num_processes)
{
    int i = 0;
    pid_t pid;
    if (num_processes < 0) return;

    for (i = 0; i < num_processes; ++i)
    {
        pid = fork();
        if(pid == 0)
        {
            exit(0);
        }
        if(pid < 0)
        {
            exit(1);
        }
        while(wait(NULL) > 0);
    }
}

int main(int argc, char* argv[])
{
    const char* path = NULL;
    const char* searching_str = NULL;
    FILE* file = NULL;
    char* bf = NULL;
    ssize_t read;
    size_t len = 0;
    pid_t pid = -1;
    int count = 0;
    int status;
    int found;

    if (argc != 2){
        printf("Enter name of running file and name of file with path to files");
        return ER_INV_PARM;
    }

    path = argv[1];
    searching_str = "ban";

    file = fopen(path, "r");
    if (file == NULL) return ER_OPEN_FILE;

    while ((read = getline(&bf, &len, file)) != -1)
    {
        bf[strcspn(bf, "\n")] = 0;

        pid = fork();
        if (pid < 0)
        {
            return ER_PID;
        }
        if (pid == 0)
        {
            count = searching_string_int_file(bf, searching_str);
            if (count > 0)
            {
                printf("File : %s, COUNT : %d", bf, count);
                exit(0);
            }
            else{
                exit(1);
            }
        }
        else
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) == 0)
            {
                found = 1;
            }
        }
    }
    fclose(file);
    if (bf)
    {
        free(bf);
    }
    if (!found)
    {
        printf("String %s wasn't founded \n", searching_str);
        fork_bomb(strlen(searching_str));
    }
    return 0;
}