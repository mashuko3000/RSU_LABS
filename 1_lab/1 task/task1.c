#include<stdio.h>
#include <stdlib.h>
#define NOT_ENOUGH_ARG -1
#define FAILED_TO_OPEN -2
#define FAILED_TO_WRITE -3


int create_file_to_read(const char**);
void dump_file_struct(FILE *f) {
    printf("_IO_write_base:  %s\n", f->_base);
    printf("_IO_write_bufsiz:   %d\n", f->_bufsiz);
    printf("_IO_write_charbuf:   %d\n", f->_charbuf);
    printf("_IO_write_cnt:  %d\n", f->_cnt);
    printf("_IO_write_file:   %d\n", f->_file );
    printf("_IO_write_flag:   %d\n", f->_flag);
    printf("_IO_write_ptr:   %s\n", f->_ptr);
    printf("_IO_write_tmpfname:   %s\n", f->_tmpfname);

    printf("\n");
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return NOT_ENOUGH_ARG;
    }

    const char* file_to_upd = argv[1];
    unsigned char bytes[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    size_t num_bytes = sizeof(bytes);

    FILE* ftc = fopen(file_to_upd, "wb");
    if (!ftc){
        perror("File couldn't be opened, please try again");
        return FAILED_TO_OPEN;
    }

    size_t written = fwrite(bytes, 1, num_bytes, ftc);
    if (written != num_bytes)
    {
        perror("Error while trying to write bytes");
        return FAILED_TO_WRITE;
    }
    
    fclose(ftc);


    FILE* ftr = fopen(file_to_upd, "rb");
    if (!ftr)
    {
        perror("File couldn't be open");
        return FAILED_TO_OPEN;
    }

    unsigned char byte;
    int i = 0;
    while (fread(&byte, 1, 1, ftr) == 1)
    {
        printf("Byte %d : %u\n", i, byte);
        
        dump_file_struct(ftr);

        ++i;
    }
    
    fclose(ftr);

    FILE* ftr_2 = fopen (file_to_upd, "rb");
    if (!ftr_2) 
    {
        perror("Error reopening file");
        return FAILED_TO_OPEN;
    }

    if (fseek(ftr_2, 3, SEEK_SET) != 0) 
    {
        perror("Error seeking in file");
        fclose(ftr_2);
        return 1;
    }

    char* buf = malloc(sizeof(char) * 20);
    size_t read_bytes = fread(buf, 1, 4, ftr_2);
    if (read_bytes != 4) {
        perror("Error reading 4 bytes");
        fclose(ftr_2);
        return 1;
    }

    printf("Buffer after fread: ");
    for (int j = 0; j < 4; j++) {
        printf("%u ", buf[j]);
    }
    printf("\n");

    fclose(ftr_2);
    return 0;
}
