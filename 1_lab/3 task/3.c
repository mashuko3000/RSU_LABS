#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERR_INVALID -1
#define ERR_OPEN_FILE -2
#define ERR_MALLOC -3

int copy_data (const char* source_path, const char* dest_path)
{
    FILE *source_file;
    FILE *dest_file;
    size_t capacity = 4096;
    char* buffer;
    size_t bytes_read;
    char* temp;
    size_t bytes_written;


    source_file = fopen(source_path, "rb");
    if (!source_file)
    {
        return ERR_OPEN_FILE;
    }

    dest_file = fopen(dest_path, "wb");
    if (!dest_file)
    {
        return ERR_OPEN_FILE;
    }

    buffer = (char*)malloc(sizeof(char) * capacity);
    if (buffer == NULL)
    {
        fclode(source_file);
        fclode(dest_file);
        return ERR_MALLOC;
    }

    while((bytes_read = fread(buffer, 1, capacity, source_file)) > 0)
    {
        if(bytes_read == capacity){
            temp = (char*)realloc(buffer, sizeof(char) * 2 * capacity);
            if (temp == NULL)
            {
                free(buffer);
                fclode(source_file);
                fclode(dest_file);
                return ERR_MALLOC;
            }
            buffer = temp;
            capacity*=2;
        }
        bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_read != bytes_written)
        {
            free(buffer);
            fclode(source_file);
            fclode(dest_file);
            return EXIT_FAILURE;
        }
    }

    if(ferror(source_file))
    {
        free(buffer);
        fclode(source_file);
        fclode(dest_file);
        return EXIT_FAILURE;
    }
    free(buffer);
    fclode(source_file);
    fclode(dest_file);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    int result;
    if (argc != 3)
    {
        return ERR_INVALID;
    }

    const char* source_path = argv[1];
    const char* dest_path = argv[2];

    result = copy_data(source_path, dest_path);
    return result;
}