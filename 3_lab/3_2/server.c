#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#define FIFO_NAME "/tmp/my_fifo_2526"
#define MAX_LEN 100
#define MIN_LEN 1
#define TARGET_COUNT 5

void check_error(int res, const char* msg);
void safe_close(int fd, const char* fd_name);
void safe_unlink();
int main()
{
    int fd;
    int length_counts[101] = {0};
    int max_count = 0;

    char buffer[101];

    printf("Server: Creating FIFO: %s\n", FIFO_NAME);
    if(mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST)
    {
        check_error(-1, "mkfifo");
    }


    printf("Server: Waiting for client connection (opening FIFO for read) \n");
    fd = open(FIFO_NAME, O_RDONLY);
    check_error(fd, "open FIFO for read");
    printf("Server: Client connected. Starting to recieve messages. \n");

    ssize_t bytes_read;
    int total_messages = 0;

    while(max_count < TARGET_COUNT)
    {
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1)
        {
            safe_close(fd, "FIFO read end");
            safe_unlink();
            check_error(-1, "read from FIFO");
        }
        else if (bytes_read == 0)
        {
            printf("Server: Client disconnectrd (EOF recieved).\n");
            break;
        }
        else
        {
            total_messages++;
            buffer[bytes_read] = '\0';
            size_t current_len = strlen(buffer);

            if(current_len < MIN_LEN || current_len > MAX_LEN)
            {
                printf("Server: Recieved invalid length(%d). Ignoring. \n", (int)current_len);
                continue;
            }

            length_counts[current_len]++;

            if(length_counts[current_len] > max_count)
            {
                max_count = length_counts[current_len];
            }

            buffer[current_len] = '\0';
            printf("Server: Message #%d recieved. Len: %ld. Count: %d/%d \n", total_messages, current_len, length_counts[current_len], TARGET_COUNT);
        }
    }
    printf("\n Server: Termination condition met (Max Count: %d >= Target: %d). \n", max_count, TARGET_COUNT);
    safe_close(fd, "FIFO read end");
    safe_unlink();

    return 0;
}

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

void safe_unlink()
{
    if(unlink(FIFO_NAME) == -1)
    {
        if(errno != ENOENT)
        {
            perror("Error unlinking FIFO");
        }
    }
    else
    {
        printf("server: Successfully unlinked FIFO: %s \n", FIFO_NAME);
    }
}