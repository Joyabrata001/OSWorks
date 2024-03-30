#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Hello World!\n");
    }
    else if (argc == 2)
    {
        printf("Hello %s\n", argv[1]);
    }
    else
    {
        exit(9);   // 10 signals incorrect usage of command line arguments
    }
    exit(0);
}