#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <ctype.h>
#include <string.h>

#define OK 0
#define ERR_FILE_OPEN 1
#define ERR_MEMORY 2
#define ERR_INVALID_ARGS 3
#define ERR_PARSEHEX 4
#define ERR_FILE_TOO_SMMALL 5

void perform_xor8(FILE* file, uint8_t* result)
{
    if(file == NULL || result == NULL) return;
    uint8_t byte;
    *result = 0;
    while(fread(&byte, 1, 1, file))
    {
        *result^=byte;
    }
}

int is_prime(unsigned int n)
{
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    unsigned int i = 0;

    for (i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n %(i+2) == 0) return 0;
    }
    return 1;
}

int perform_xorodd(FILE* file, size_t* size, uint32_t* result)
{
    if (file == NULL || result == NULL) return ERR_INVALID_ARGS;

    uint8_t block[4];
    size_t total_read = 0;
    *result = 0;

    while(fread(block, 1, 4, file) == 4)
    {
        int j = 0;
        int has_prime = 0;
        for(j = 0; j<4; ++j)
        {
            if (is_prime(block[j]))
            {
                has_prime = 1;
                break;
            }
        }
        if(has_prime)
        {
            uint32_t block_val;
            memcpy(&block_val, block, sizeof(uint32_t));
            *result ^= block_val;
        }
    }

    *size = total_read;
    if(ferror(file)) return ERR_FILE_OPEN;
    return OK;
}

int perform_mask(FILE* file, size_t *size, uint32_t mask, uint32_t *count)
{
    if (file == NULL || count == NULL) return ERR_INVALID_ARGS;

    uint8_t block[4];
    size_t total_read = 0;
    *count = 0;

    while(fread(block, 1, 4, file) == 4)
    {
        uint32_t val;
        memcpy(&val, block, sizeof(uint32_t));
        if((val&mask) == mask)
        {
            ++(*count);
        }
        total_read += 4;
    }
    *size = total_read;
    if(ferror(file)) return ERR_FILE_OPEN;
    return OK;
}


int parsing_hex(const char *hex_str, uint32_t* mask)
{
    if ((hex_str == NULL) || (*hex_str == '\0' || mask == NULL)) return -1;

    *mask = 0;
    const char *ptr = hex_str;
    while(*ptr == ' ')
    {
        ++ptr;
    }

    while (*ptr != '\0')
    {
        uint32_t digit;
        if(*ptr >= '0' && *ptr <= '9')
        {
            digit = *ptr - '0';
        }
        else if (*ptr >= 'a' && *ptr <= 'f')
        {
            digit = *ptr - 'a' + 10;
        }
        else if (*ptr >= 'A' && *ptr <= 'F')
        {
            digit = *ptr - 'A' + 10;
        }
        else
        {
            return ERR_PARSEHEX;
        }

        if (*mask > 0x0FFFFFFF)
        {
            return ERR_PARSEHEX;
        }
        *mask = (*mask << 4) | digit;
        ++ptr;
    }

    return OK;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        return ERR_INVALID_ARGS;
    }

    if (strcmp(argv[2], "mask") == 0 && argc != 4) return ERR_INVALID_ARGS;
    if (strcmp(argv[2], "mask") == 0 && argc != 3) return ERR_INVALID_ARGS;

    FILE* file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        return ERR_FILE_OPEN;
    }

    const char *flag = argv[2];
    uint8_t *data = NULL;
    size_t size = 0;
    uint8_t xor8_result;
    uint32_t xorodd_result;
    uint32_t mask;
    uint32_t mask_count;
    int err = 0;

    if (strcmp(flag, "xor8") == 0) 
    {
        perform_xor8(file, &xor8_result);
        if (ferror(file))
        {
            fclose(file);
            return ERR_FILE_OPEN;
        }
        printf("RESULT FLAG xor8: %02X\n", xor8_result);
    }


    else if(strcmp(flag, "xorodd")==0)
    {
        err = perform_xorodd(file, &size, &xorodd_result);
        if (err != OK)
        {
            fclose(file);
            return err;
        }

        if (size < 4 && size > 0)
        {
            fprintf(stderr, "Error : File too small for this operation");
            fclose(file);
            return ERR_FILE_TOO_SMMALL;
        }
        if (size%4 != 0) 
        {
            fprintf(stderr, "WARNING File size not multiple of 4 bytes, ignoring trailing bytes");
        }
        printf("RESULT FLAG xorodd: %08X\n", xorodd_result);
    }

    else if(strcmp(flag, "mask") == 0)
    {
        err = parsing_hex(argv[3], &mask);
        if (err != OK)
        {
            fclose(file);
            return err;
        }
        err = perform_mask(file, &size, mask, &mask_count);
        if (err != OK)
        {
            fclose(file);
            return err;
        }
        if (size < 4)
        {
            fclose(file);
            return ERR_FILE_TOO_SMMALL;
        }
        if (size % 4 != 0)
        {
            fprintf(stderr, "WARNING File size not multiple of 4 bytes, ignoring trailing bytes");
        }
        printf("%u \n", mask_count);
    }

    else
    {
        fprintf(stderr, "Unknown flag: %s \n", flag);
        free(data);
        return ERR_INVALID_ARGS;
    }
    fclose(file);
    return OK;
}