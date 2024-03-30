#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int fibonacci(int n)
{
    if (n <= 1)
        return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
int main(int argc, char *argv[])
{
    if (argc == 1)
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
        printf("Fibonacci(%d): %d\n", n, fibonacci(n));
        exit(0);
    }
}
