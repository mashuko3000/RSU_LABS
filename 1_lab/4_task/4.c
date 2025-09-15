#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <ctype.h>
#include <string.h>

void perform_xor8(const uint8_t *data, size_t size, uint8_t* result)
{
    if(data == NULL || result == NULL) return;
    *result = 0;
    for (int i = 0; i < size; ++i)
    {
        (*result) ^= data[i];
    }
}

int is_prime(unsigned char n)
{
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (unsigned char i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n %(i+2) == 0) return 0;
    }
    return 1;
}

void perform_xorodd(const uint8_t *data, size_t size, uint32_t* result)
{
    if (data == NULL || result == NULL) return;
    (*result) = 0;
    size_t num_blocks = size/4;
    for (size_t i = 0; i < size; ++i)
    {
        const uint8_t *block = &data[i*4];
        int has_prime = 0;
        for (int j = 0; j < 4; ++j)
        {
            if(is_prime(block[j]))
            {
                has_prime = 1;
                break;
            }
        }
        if (has_prime)
        {
            uint32_t block_val = *(const uint32_t *)block;
            (*result)^=block_val;
        }
    }
}

void perform_mask(const uint8_t *data, size_t size, uint32_t mask, uint32_t *count)
{
    *count = 0;
    size_t num_blocks = size/4;
    for(size_t i = 0; i<num_blocks; ++i)
    {
        const uint8_t *block = &data[i*4];
        uint32_t val = *(const uint32_t *)block;
        if((val & mask) == mask)
        {
            ++(*count);
        }
    }
}

int parsing_hex(const char *hex_str, uint32_t* mask)
{
    if ((hex_str == NULL) || (*hex_str == '\0')) return -1;

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
            return EXIT_FAILURE;
        }

        if (*mask > 0x0FFFFFFF)
        {
            return EXIT_FAILURE;
        }
        *mask = (*mask << 4) | digit;
        ++ptr;
    }

    return EXIT_SUCCESS;
}

int read_file (const char* filepath, uint8_t **data, size_t *size)
{
    FILE* file = fopen(filepath, "rb");
    if (file == NULL)
    {
        return EXIT_FAILURE;
    }

    size_t capacity = 30;
    *data = malloc(sizeof(uint8_t) * capacity);
    if(*data == NULL)
    {
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t total_read = 0;
    size_t bytes_read = 0;
    while((bytes_read = fread(*data + total_read, 1, capacity - total_read, file)) > 0)
    {
        total_read += bytes_read;
        if(total_read == capacity)
        {
            uint8_t *tmp = realloc(data, sizeof(uint8_t) * 2 * capacity);
            if (tmp == NULL)
            {
                fclose(file);
                free(*data);
                return EXIT_FAILURE;
            }
            *data = tmp;
        }
    }

    if (ferror(file))
    {
        free(*data);
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);
    *size = total_read;
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc < 3 || (strcmp(argv[2], "mask") == 0) || argc != 4)
    {
        return EXIT_FAILURE;
    }

    const char*input_path = argv[1];
    const char *flag = argv[2];

    uint8_t *data = NULL;
    size_t size = 0;
    if(read_file(input_path, &data, &size) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    if (strcmp(flag, "xor8") == 0) 
    {
        uint8_t xor8_result;
        perform_xor8(data, size, &xor8_result);
        printf("RESULT FLAG xor8: %02X\n", xor8_result);
    }
    else if(strcmp(flag, "xorodd")==0)
    {
        if (size < 4 && size > 0)
        {
            fprintf(stderr, "Error : File too small for this operation");
            return EXIT_FAILURE;
        }
        if (size%4 != 0) 
        {
            fprintf(stderr, "WARNING File size not multiple of 4 bytes, ignoring trailing bytes");
        }
        uint32_t xorodd_result;
        perform_xorodd(data, size, &xorodd_result);
        printf("RESULT FLAG xorodd: %08X\n", xorodd_result);
    }
    else if(strcmp(flag, "mask") == 0)
    {
        if(argc != 4)
        {
            return EXIT_FAILURE;
        }
        else
        {
            uint32_t mask;
            if(parsing_hex(argv[3], &mask) != EXIT_SUCCESS)
            {
                return EXIT_FAILURE;
            }
            else
            {
                if (size < 4 && size > 0)
                {
                    return EXIT_FAILURE;
                }
                else if (size % 4 != 0)
                {
                    fprintf(stderr, "WARNING File size not multiple of 4 bytes, ignoring trailing bytes");
                }
                uint32_t mask_count;
                perform_mask(data, size, mask, &mask_count);
                printf("%u \n", mask_count);
            }
        }
    }
    else{
        fprintf(stderr, "Unknown flag: %s \n", flag);
    }
    return EXIT_SUCCESS;
}