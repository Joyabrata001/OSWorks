#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int n = argc;
    int status;
    char *myargv[] = {NULL};

    for (int i = 1; i < n; ++i)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork() failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            status = execve(argv[i], myargv, NULL);

            if (status == -1)
            {
                fprintf(stderr, "Exec failed for %s\n", argv[i]);
                perror("execve() failed");
                exit(EXIT_FAILURE);
            }

            exit(status);
        }
    }

    for (int i = 1; i < n; ++i)
    {
        pid_t child_pid = wait(&status);

        if (child_pid == -1)
        {
            perror("wait() failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            printf("Child process %d exited with status: %d\n", child_pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child process %d terminated by signal %d\n", child_pid, WTERMSIG(status));
        }
    }

    return 0;
}