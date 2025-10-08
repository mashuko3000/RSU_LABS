#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/my_fifo_2526"
#define MIN_LEN 1
#define MAX_LEN 100
#define MAX_BUFFER_SIZE (MAX_LEN + 1)

void check_error(int res, const char* msg)
{
    if(res == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
void safe_close(int fd, const char* fd_name)
{
    if(fd>=0 && close(fd) == -1)
    {
        char err_msg[100];
        snprintf(err_msg, 100, "Error closing %s", fd_name);
        perror(err_msg);
    }
}

size_t generate_random_string(char* buffer, int length)
{
    const char char_set[] = "abcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < length; ++i)
    {
        int index = rand() % (sizeof(char_set) - 1);
        buffer[i] = char_set[index];
    }
    buffer[length] = '\0';

    return (size_t)length +1;
}

int main()
{
    int fifo_fd = -1;

    srand(time(NULL) ^ getpid());

    char* buffer = (char*)malloc(sizeof(char) * MAX_BUFFER_SIZE);
    if (buffer == NULL)
    {
        fprintf(stderr, "Client: ERROR: Failed to allocate memory. \n");
        return -1;
    }

    printf("Client: Waiting to open FIFO for write: %s\n", FIFO_PATH);
    fifo_fd = open(FIFO_PATH, O_WRONLY);
    check_error(fifo_fd, "open FIFO for write");
    /*
    if (fifo_fd == -1)
    {
        free(buffer);
        check_error(-1, "open FIFO for write");
    }*/
    printf("Client: Successfully connected to server. \n");

    int msg_count = 0;

    while(1)
    {
        int length = (rand() % (MAX_LEN - MIN_LEN + 1)) + MIN_LEN;
        size_t bytes_to_send = generate_random_string(buffer, length);

        ssize_t bytes_written = write(fifo_fd, buffer, bytes_to_send);

        if (bytes_written == -1)
        {
            if(errno == EPIPE)
            {
                printf("\n Client: EPIPE received. Server likely terminated connection. \n");
            }
            else
            {
                perror("write to FIFO");
            }
            break;
        }
        msg_count++;
        printf("Client: Sent Msg ##%d (Len: %zu). Sent %zd bytes. \n", msg_count, bytes_to_send - 1, bytes_written);
        sleep(1);
    }

    safe_close(fifo_fd, "FIFO write end");
    free(buffer);
    printf("Client: Finished. Total messages sent: %d\n", msg_count);
    return 0;
}