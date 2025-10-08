#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE "Hello from Parent Process"

void check_error(int res, const char* message);
void safe_close(int fd, const char* fd_name);

int main()
{
    int pipe_fd[2];
    check_error(pipe(pipe_fd), "pipe");

    pid_t pid = fork();
    check_error(pid, "fork");

    if (pid > 0)
    {
        safe_close(pipe_fd[0], "parent read end");
        printf("Parent (PID: %d) is writting to pipe...\n", getpid());
        ssize_t bytes_written = write(pipe_fd[1], MESSAGE, strlen(MESSAGE) +1);
        check_error((int)bytes_written, "write");
        printf("Parent wrote %zd bytes to pipe.\n", bytes_written);
        safe_close(pipe_fd[1], "parent write end");
        check_error(wait(NULL), "wait");   
    }
    else
    {
        int capacity = 512;
        char* buffer = (char*)malloc(sizeof(char) * capacity);
        if(buffer == NULL)
        {
            fprintf(stderr, "Child: ERROR: Failed to allocate memory.\n");
            exit(EXIT_FAILURE);
        }
        safe_close(pipe_fd[1], "child write end");
        printf("Child (PID: %d) is waiting to read from pipe \n", getpid());

        ssize_t bytes_read = read(pipe_fd[0], buffer, capacity);
        check_error((int)bytes_read, "read");

        if(bytes_read > 0)
        {
            if (bytes_read < capacity)
            {
                buffer[bytes_read] = '\0';
            }
            printf("Child read %zd bytes. Recieving message: **%s**\n", bytes_read, buffer);
        }
        else
        {
            printf("Child recieved EOF. \n");
        }
        free(buffer);
        safe_close(pipe_fd[0], "child read end");
        exit(EXIT_SUCCESS);
    }
}

void check_error(int res, const char* message)
{
    if (res == -1)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

void safe_close(int fd, const char* fd_name)
{
    if (close(fd) == -1)
    {
        char err_msg[100];
        snprintf(err_msg, 100, "Error closing %s", fd_name);
        perror(err_msg);
    }
    else
    {
        printf("Closed %s (FD: %d) \n", fd_name, fd);
    }
}