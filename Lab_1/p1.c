#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int factorial(int num)
{
    int fact = 1;
    for (int i = 1; i <= num; ++i)
    {
        fact *= i;
    }
    return fact;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <positive integer>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n <= 0)
    {
        printf("Please enter a positive integer next time.\n");
        return 1;
    }

    for (int i = 1; i <= n; ++i)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            printf("Child %d : Factorial of %d is %d\n", i, i, factorial(i));
            exit(0);
        }
    }

    return 0;
}
