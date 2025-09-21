#include<stdio.h>
#include<unistd.h>

void task1()
{
    int pid;
    pid = fork();
    printf("%d\n", pid);
}

void task2()
{
    fork();
    printf("Hi\n");
    fork();
    printf("Hi\n");
}

void task3()
{
    if(fork() || fork()) fork();
    printf("forked! %d", fork());
}

void task4()
{
    if (fork() && (!fork()))
    {
        if (fork() || fork()) fork();
    }
    printf("52\n");
}

int main()
{
    //task1();
    //task2();
    //task3();
    //task4();
    return 0;
}