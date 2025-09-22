#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>


const char* get_file_ext(const char* filename)
{
    const char* dot;

    if (filename == NULL) return "";

    dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";

    return dot + 1;
}

int safe_pth_concat(char* dest, size_t dest_size, const char* dir, const char* name)
{
    int result;

    if(dest == NULL || dir == NULL || name == NULL)
    {
        return -1;
    }
    result = snprintf(dest, dest_size, "%s/%s", dir, name);
    
    if (result < 0 || result >= dest_size)
    {
        fprintf(stderr, "Error : Path concatenation failed. \n");
        return -1;
    }

    return 0;
}

void list_files(const char* dirname, int* recmin,int* recmax, int depth)
{
    DIR* dir;
    struct dirent *ent;
    struct stat file_info;
    int capacity = 1024;
    char* tmp = NULL;
    char* filepath = (char*)malloc(sizeof(char)*capacity);
    if (filepath == NULL)
    {
        fprintf(stderr, "Memory allocation error. \n");
        return;
    }

    if (depth > *(recmax)) return;
    
    dir = opendir(dirname);

    if (dir == NULL) return;

    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0)) continue;

        if (safe_pth_concat(filepath, capacity, dirname, ent->d_name) != 0)
        {
            tmp = (char*)realloc(filepath, sizeof(char) * 2 * capacity);
            if (tmp == NULL)
            {
                fprintf(stderr, "Memory reallocation error. \n");
                free(filepath);
                closedir(dir);
                return;
            }

            capacity*=2;
            filepath = tmp;
            
            if (safe_pth_concat(filepath, capacity, dirname, ent->d_name) != 0)
            {
                fprintf(stderr, "Fatal error : path concatenation failed even after reallocating. \n");
                free(filepath);
                closedir(dir);
                return;
            }
        }

        if (stat(filepath, &file_info) == -1) continue;

        if (S_ISDIR(file_info.st_mode))
        {
            if (depth >= *(recmin))
            {
                printf("Directory : %s\n", filepath);
            }
            list_files(filepath, recmin, recmax, depth + 1);
        }
        else{
            if (depth >= *(recmin))
            {
                const char* ext = get_file_ext(ent->d_name);
                printf("File: %s, Extension: %s, Path: %ld\n", ent->d_name, ext, file_info.st_ino);
            }
        } 
    }
    closedir(dir);
}

int main(int argc, char* argv[])
{
    if (argc < 4) return -1;
    
    int recmax = atoi(argv[2]);
    int recmin = atoi(argv[1]);

    if (recmin < 0 || recmin > recmax || recmax < 0) return -1;

    for(int i = 3; i < argc; i++){
        printf("Directory : %s\n", argv[i]);
        list_files(argv[i], &recmin, &recmax, 0);
    }

    return 0;
}