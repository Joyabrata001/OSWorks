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

int MIN_SIZE_OF_ARRAY = 5;
int MAX_SIZE_OF_ARRAY = 10;

struct task
{

    int data[100];

    int size;

    pid_t worker_pid; /* The pid of the worker process that is sorting (or has sorted)  the size number of elements of the array data. */

    int status;
    // status : status implication
    // status =  0 : at present there is nothing in the array data[ ] that needs to be sorted.
    // status =  1 : server has put some numbers in the array data[ ] that need to be sorted.
    // status =  2 : worker having pid = worker_pid has started sorting size number of elements of the array data[ ].
    // status =  3 : worker having pid = worker_pid has sorted size number of elements of the array data[ ].
    // status =  4 : server is using the sorted elements of the array data[ ].
    // status = -1 : server ended.
};

int shmid_task = -1; /* to store the id of the shared memory segment as returned by shmget(). this variable is defined globally so that signal handler can access it for releasing the shared memory segment. */

typedef void (*sighandler_t)(int);
void releaseSHM(int signum)
{
    int status;
    // int shmctl(int shmid, int cmd, struct shmid_ds *buf); /* Read the manual for shmctl() */
    status = shmctl(shmid_task, IPC_RMID, NULL);
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

int main()
{
    /* Install signal handler */
    // sighandler_t signal(int signum, sighandler_t handler);
    sighandler_t shandler;
    shandler = signal(SIGINT, releaseSHM);

    // Create a unique key using ftok
    // ftok - convert a pathname and a project identifier to a System V IPC key
    // key_t ftok(const char *pathname, int proj_id);
    key_t key = ftok("/tmp/", 12);
    if (key == -1)
    {
        perror("ftok() failed: ");
        exit(1);
    }

    // Check if the shared memory segment has been created
    if (shmid_task == -1)
    {
        // int shmget(key_t __key, size_t __size, int __shmflg)
        shmid_task = shmget(key, sizeof(struct task), IPC_CREAT | 0777);
        if (shmid_task == -1)
        {
            perror("shmget() failed for the task: ");
            exit(1);
        }
        // printf("shmget() returns shmid_task = %d.\n", shmid_task);
    }

    // printf("shmget() returns shmid_task = %d.\n", shmid_task);
    // void *shmat(int shmid, const void *shmaddr, int shmflg)
    struct task *solve = shmat(shmid_task, NULL, 0);
    if (solve == (void *)-1)
    {
        perror("shmat() failed for solve: ");
        exit(1);
    }

    while (solve->status != -1) /* while server has not terminated */
    {
        if (solve->status == 0) /* array empty */
        {
            srand(time(NULL));
            solve->size = rand() % (MAX_SIZE_OF_ARRAY - MIN_SIZE_OF_ARRAY + 1) + MIN_SIZE_OF_ARRAY;

            printf("\nUnsorted array: ");
            for (int i = 0; i < solve->size; ++i)
            {
                solve->data[i] = rand() % 100 + 1;
                printf("%d ", solve->data[i]);
            }
            printf("\n");
            solve->status = 1;
        }
        else if (solve->status == 1)
        {
            printf("No worker working on the array..\n");
            while (solve->status == 1)
                ;
        }
        else if (solve->status == 2)
        {
            printf("Worker is working on the array.\n");
            while (solve->status == 2)
                ;
        }
        else if (solve->status == 3) /* worker working on the array */
        {
            solve->status = 4;
            printf("\nSorted Array: ");
            for (int i = 0; i < solve->size; ++i)
            {
                printf("%d ", solve->data[i]);
            }
            printf("\t Worker PID = %d\n", solve->worker_pid);
        }
        else if (solve->status == 4) /* server using the sorted data */
        {
            solve->status = 0;
            getchar();
        }
    }
    exit(0);
}