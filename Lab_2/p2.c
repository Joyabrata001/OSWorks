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

    for (int i = 0; i < n; ++i)
    {
        printf("%s %s\n", executable[i], argument[i][1]);
    }

    printf("\n\n\nPress enter to move on to the first executable\n");
    getchar();
    system("clear");

    for (int i = 1; i <= n; ++i)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            printf("\nExecutable %d of %d", i, n);
            printf("\n\n----------Executing %s %s----------\n\n", executable[i - 1], argument[i - 1][1]);
            if (execve(executable[i - 1], argument[i - 1], NULL) == -1)
            {
                perror("\n\n\nExecution failed");
                exit(11);
            }
        }

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            printf("\n\n\nBack to parent process with status %d", WEXITSTATUS(status));
            switch (WEXITSTATUS(status))
            {
            case 0:
                printf("\n\n%s %s executed successfully", executable[i - 1], argument[i - 1][1]);
                break;
            case 9:
                printf("\n\nImproper argument(s) for executable %s", executable[i - 1]);
                break;
            case 11:
                printf("\n\n%s %s execution failed", executable[i - 1], argument[i - 1][1]);
                break;
            }
        }

        printf("\n\n\nPress enter to move on to the next executable\n");
        getchar();
        system("clear");
    }
}