#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Function to calculate factorial
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
    if (argc != 3)
    {
        printf("Usage: %s <positive integer1> <positive integer2>\n", argv[0]);
        exit(1);
    }
    int n1 = atoi(argv[1]);
    int n2 = atoi(argv[2]);

    if (n1 <= 0 || n2 <= 0 || n1 > n2)
    {
        printf("Enter valid positive integers where n1 <= n2.\n");
        return 1;
    }

    int fact = factorial(n1 - 1);
    for (int i = n1; i <= n2; ++i)
    {
        pid_t pid = fork(); // this creates a fork with fact value update (i.e fact*=i)

        if (pid == 0)
        {
            fact = fact * i;
            printf("Child %d : Factorial of %d is %d\n", i - n1 + 1, i, fact);
            exit(fact);
        }
        else
        {
            int status;
            // pid_t waitpid(pid_t pid, int *wstatus, int options);
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                fact = WEXITSTATUS(status);
        }
    }

    return 0;
}