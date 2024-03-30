#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int m, n, r;
    printf("\nEnter the dimensions of A: ");
    scanf("%d %d", &m, &n);
    printf("\nB must have %d rows, enter the number of cols of B: ", n);
    scanf("%d", &r);

    int A[m][n], B[n][r], C[m][r], D[m][r];

    printf("\nEnter the elements of A(%d * %d) row-wise:\n", m, n);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            scanf("%d", &A[i][j]);

    printf("\nEnter the elements of B(%d * %d) row-wise:\n", n, r);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < r; ++j)
            scanf("%d", &B[i][j]);

    printf("\nA:\n");
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
            printf("%d ", A[i][j]);
        printf("\n");
    }

    printf("\nB:\n");
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < r; ++j)
            printf("%d ", B[i][j]);
        printf("\n");
    }

    printf("\nWhat multiplication result should be:\n");
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < r; ++j)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k)
                C[i][j] += A[i][k] * B[k][j];
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    pid_t pid[m][r];

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < r; ++j)
        {
            pid[i][j] = fork();

            if (pid[i][j] == 0)
            {
                int sum = 0;
                for (int k = 0; k < n; ++k)
                {
                    sum += (A[i][k] * B[k][j]);
                }
                // printf("\nSum = %d", sum);
                exit(sum);
            }
        }
    }

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < r; ++j)
        {
            int status;
            waitpid(pid[i][j], &status, 0);

            if (WIFEXITED(status))
            {
                D[i][j] = WEXITSTATUS(status);
            }
        }
    }

    printf("\nMultiplication result I am getting:\n");
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < r; ++j)
        {
            printf("%d ", D[i][j]);
        }
        printf("\n");
    }
}