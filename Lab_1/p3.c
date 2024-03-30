#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int calculateTerm(int coefficient, int exponent, int x)
{
    int term_result = coefficient;
    for (int j = 0; j < exponent; j++)
    {
        term_result *= x;
    }
    return term_result;
}

int main(int argc, char *argv[])
{
    int n = argc - 3; // -1 for ./P3 and -1 for x (therefore arg = n + 2)
    int coeff[n + 1]; // an to a0 ==> n+1 coeffs

    for (int i = 0; i <= n; i++)
        coeff[i] = atoi(argv[i + 1]);

    int x = atoi(argv[argc - 1]);

    printf("Polynomial: ");
    for (int i = 0; i <= n; i++)
    {
        if (coeff[i] == 0)
            continue;
        else
        {
            printf("%+dx^%d", coeff[i], n - i);
        }
    }
    printf("\nx = %d\n\n", x);

    int sum = 0;
    pid_t pid;

    for (int i = 0; i <= n; ++i)
    {
        pid = vfork();

        if (pid == 0)
        {
            sum += calculateTerm(coeff[i], n - i, x);
            exit(0);
        }
    }

    printf("Sum of the terms: %d\n", sum);
    return 0;
}

/*
 * vfork blocks parent and executes child
 * fork uses copy-on-write: The pages between the parent and child, and only when either process modifies a page is a separate copy made for that process
 * vfork accesses the memory and stack of the the parent
 */