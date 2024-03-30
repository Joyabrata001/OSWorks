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

int shmid_n, shmid_r; /* to store the ids of the shared memory segment as returned by shmget(). this variable is defined globally so that signal handler can access it for releasing the shared memory segment. */

typedef void (*sighandler_t)(int);
void releaseSHM(int signum)
{
    int status;
    // int shmctl(int shmid, int cmd, struct shmid_ds *buf); /* Read the manual for shmctl() */
    status = shmctl(shmid_n, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
    if (status == 0)
    {
        fprintf(stderr, "Remove shared memory for n.\n");
    }
    else if (status == -1)
    {
        fprintf(stderr, "Cannot remove shared memory for n.\n");
    }

    status = shmctl(shmid_r, IPC_RMID, NULL);
    if (status == 0)
    {
        fprintf(stderr, "Remove shared memory for r.\n");
    }
    else if (status == -1)
    {
        fprintf(stderr, "Cannot remove shared memory for r.\n");
    }

    // int kill(pid_t pid, int sig);
    // pid  = 0 => sig sent to every process in the process grou[]
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
        fprintf(stderr, "kill returned wrong value.\n");
    }
}

int calculateFactorial(int n)
{
    if (n == 0 || n == 1)
    {
        return 1;
    }
    else
    {
        return n * calculateFactorial(n - 1);
    }
}

int main()
{
    int status;
    pid_t pid = 0;

    sighandler_t shandler;

    /* Install signal handler */
    // sighandler_t signal(int signum, sighandler_t handler);
    shandler = signal(SIGINT, releaseSHM);

    /* Create shared memory for n and check for error */
    // int shmget(key_t key, size_t size, int shmflg);
    shmid_n = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
    if (shmid_n == -1)
    {
        perror("shmget() failed for n: ");
        exit(1);
    }

    /* Create shared memory for r and check for error */
    shmid_r = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
    if (shmid_r == -1)
    {
        perror("shmget() failed for r: ");
        exit(1);
    }

    printf("shmget() returns shmid_n = %d and shmid_r = %d.\n", shmid_n, shmid_r);
    printf("\n----- Parent assigns random value to n. Child calculates r. Both print the values of n and r -----\n\n");

    pid = fork();
    if (pid == 0) /* Child process */
    {
        int n, r;

        /* Attach to shared memory for n and check for error */
        // void *shmat(int shmid, const void *shmaddr, int shmflg)
        int *pn = shmat(shmid_n, NULL, 0);
        if (pn == (void *)-1)
        {
            perror("shmat() failed for n: ");
            exit(1);
        }

        /* Attach to shared memory for r and check for error */
        int *pr = shmat(shmid_r, NULL, 0);
        if (pr == (void *)-1)
        {
            perror("shmat() failed for r: ");
            exit(1);
        }

        while (1)
        {
            n = *pn;
            r = calculateFactorial(n);
            *pr = r;

            // printf("PID = %d \t Child: Factorial of %d is %d.\n", getpid(), n, r);
            printf("PID = %d [Child ] \t n = %d \t r = %d\n", getpid(), n, r);

            sleep(2);
        }

        exit(0);
    }
    else /* Parent process */
    {
        int n, r;

        /* Attach to shared memory for n */
        int *pn_parent = shmat(shmid_n, NULL, 0);
        if (pn_parent == (void *)-1)
        {
            perror("shmat() failed for n (parent): ");
            exit(1);
        }

        /* Attach to shared memory for r */
        int *pr_parent = shmat(shmid_r, NULL, 0);
        if (pr_parent == (void *)-1)
        {
            perror("shmat() failed for r: ");
            exit(1);
        }

        while (1)
        {
            srand(time(NULL));  // Seed random number generator
            n = rand() % 5 + 1; // Generate a random number between 1 and 7
            *pn_parent = n;
            r = *pr_parent;

            // printf("PID = %d \t Parent: Generated random value: %d (value of r = %d)\n", getpid(), n, r);
            printf("PID = %d [Parent] \t n = %d \t r = %d \t ", getpid(), n, r);
            if (calculateFactorial(n) != r)
                printf("No\n");
            else
                printf("Yes\n");
            sleep(2);
        }
        exit(0);
    }
    return 0;
}