#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int factorial(int n)
{
    if (n == 0 || n == 1)
    {
        return 1;
    }
    return n * factorial(n - 1);
}
int main(int argc, char *argv[])
{
    if (argc == 1 || argc > 2)
    {
        exit(9);
    }
    else if (argc == 2)
    {
        int n = atoi(argv[1]);
        if (n < 1)
        {
            exit(9);
        }
        printf("\nFactorial(%d) = %d\n", n, factorial(n));
        exit(0);
    }
}