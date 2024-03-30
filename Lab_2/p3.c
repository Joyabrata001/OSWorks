#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void prepare(char *executable[], char *argument[][3], char *argv[], const int n)
{
    for (int i = 1; i <= 2 * n; i += 2)
    {
        executable[i / 2] = argv[i];
        argument[i / 2][0] = argv[i];
        argument[i / 2][1] = argv[i + 1];
        argument[i / 2][2] = NULL;
    }
}

int main(int argc, char *argv[])
{
    int n = (argc - 1) >> 1;
    printf("Number of executables entered: %d\n", n);
    char *executable[n], *argument[n][3];
    prepare(executable, argument, argv, n);
    pid_t pid[n];

    for (int i = 0; i < n; ++i)
    {
        printf("%s %s\n", executable[i], argument[i][1]);
    }

    // printf("\n\n\nPress enter to move on to the first executable\n");
    getchar();

    for (int i = 1; i <= n; ++i)
    {
        pid[i - 1] = fork();

        if (pid[i - 1] == 0)
        {
            // printf("\nExecutable %d of %d", i, n);
            // printf("\n\n----------Executing %s %s----------\n\n", executable[i - 1], argument[i - 1][1]);
            if (execve(executable[i - 1], argument[i - 1], NULL) == -1)
            {
                perror("\n\n\nExecution failed");
                exit(11);
            }
        }
    }

    int status[n];
    for (int i = 0; i < n; i++)
    {
        waitpid(pid[i], &status[i], 0);
        if (WIFEXITED(status[i]))
        {
            status[i] = WEXITSTATUS(status[i]);
        }
    }
    printf("\n\n--------------------------------------------------------------------------------------------------\n\n");
    for (int i = 0; i < n; i++)
    {
        printf("PID = %d returned with exit status = %2d (for executable %s)\n", pid[i], status[i], executable[i]);
    }
    printf("\n\n11 implies abnormal termination and 0 implies normal termination\n\n");
}