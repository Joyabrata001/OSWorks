#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int shmid_matrix; /* to store the id of the shared memory segment as returned by shmget(). this variable is defined globally so that signal handler can access it for releasing the shared memory segment. */

typedef void (*sighandler_t)(int);
void releaseSHM(int signum)
{
    int status;
    // int shmctl(int shmid, int cmd, struct shmid_ds *buf); /* Read the manual for shmctl() */
    status = shmctl(shmid_matrix, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
    if (status == 0)
    {
        fprintf(stderr, "Remove shared memory for array.\n");
    }
    else if (status == -1)
    {
        fprintf(stderr, "Cannot remove shared memory for array.\n");
    }

    // int kill(pid_t pid, int sig);
    // pid  = 0 => sig sent to every process in the process group
    status = kill(0, SIGKILL);
    if (status == 0)
    {
        fprintf(stderr, "kill successful.\n");
    }
    else if (status == -1)
    {
        perror("kill failed.\n");
    }
    else
    {
        fprintf(stderr, "kill returned the wrong value.\n");
    }
}

int main(int argc, char *argv[])
{
    int n, m, p;
    printf("\nEnter the dimensions of A: ");
    scanf("%d %d", &n, &m);
    printf("\nB must have %d rows, enter the number of cols of B: ", m);
    scanf("%d", &p);

    int A[n][m], B[m][p], C[n][p];

    printf("\nEnter the elements of A(%d * %d) row-wise:\n", n, m);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            scanf("%d", &A[i][j]);

    printf("\nEnter the elements of B(%d * %d) row-wise:\n", m, p);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < p; ++j)
            scanf("%d", &B[i][j]);

    printf("\nA:\n");
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
            printf("%d ", A[i][j]);
        printf("\n");
    }

    printf("\nB:\n");
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < p; ++j)
            printf("%d ", B[i][j]);
        printf("\n");
    }

    printf("\nWhat multiplication result should be:\n");
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            C[i][j] = 0;
            for (int k = 0; k < m; ++k)
                C[i][j] += A[i][k] * B[k][j];
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    pid_t pid[n][p];

    /* Install signal handler */
    // sighandler_t signal(int signum, sighandler_t handler);
    sighandler_t shandler;
    shandler = signal(SIGINT, releaseSHM);

    /* Create shared memory for matrix and check for error */
    // int shmget(key_t key, size_t size, int shmflg);
    shmid_matrix = shmget(IPC_PRIVATE, n * p * sizeof(int), IPC_CREAT | 0666);
    if (shmid_matrix == -1)
    {
        perror("shmget() failed for the matrix ");
        exit(1);
    }
    printf("shmget() returns shmid_matrix = %d.\n", shmid_matrix);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            pid[i][j] = fork();

            if (pid[i][j] == 0) /* Child Process */
            {
                int sum = 0;
                /* Attach to shared memory for matrix and check for error */
                // void *shmat(int shmid, const void *shmaddr, int shmflg)
                int *ptr = shmat(shmid_matrix, NULL, 0);
                if (ptr == (void *)-1)
                {
                    perror("shmat() failed for matrix (child): ");
                    exit(1);
                }
                for (int k = 0; k < m; ++k)
                {
                    sum += (A[i][k] * B[k][j]);
                }
                ptr[i * p + j] = sum;
                exit(0);
            }
        }
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            int status;
            // pid_t waitpid(pid_t __pid, int *__stat_loc, int __options)
            waitpid(pid[i][j], &status, 0);
            if (!WIFEXITED(status))
            {
                printf("Child [%d][%d] terminated abnormally\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
    }

    // void *shmat(int shmid, const void *shmaddr, int shmflg)
    int *parent_ptr = shmat(shmid_matrix, NULL, SHM_RDONLY); // the flag SHM_RDONLY prevent parent process from modifying the array in the shared memory segment
    if (parent_ptr == (void *)-1)
    {
        perror("shmat() failed for matrix (child): ");
        exit(1);
    }

    printf("\nMultiplication result I am getting:\n");
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            // parent_ptr[i * p + j] = 0; ---> this line expectedly raises error (segmentation fault)
            printf("%3d ", parent_ptr[i * p + j]);
        }
        printf("\n");
    }
    return 0;
}