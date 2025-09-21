#include<stdio.h>
#include<unistd.h>

void info_proc()
{
    printf("PID : %d\n", getpid());
    printf("pPID : %d\n", getppid());
    printf("GID : %d\n", getgid());
    printf("UID : %d\n", getuid());
    printf("GID : %d\n", getgid());
    printf("eUID : %d\n", geteuid());
    printf("eGID : %d\n", getegid());
}

int main()
{
    info_proc();
    return 0;
}