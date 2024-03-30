#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <signal.h>

#include <unistd.h>

#define NO_SEM 5
int semid;

// sembuf: Structure used for argument to `semop' to describe operations such as the semaphore number, operation (increment/decrement), flags
struct sembuf Pop;
struct sembuf Vop;

// semop: Operate on System V semaphore
// int semop(int semid, struct sembuf *sops, size_t nsops);
#define P(s) semop(s, &Pop, 1)
#define V(s) semop(s, &Vop, 1)

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} setvalArg;

void removeSemaphore(int signum)
{
    int status;
    status = semctl(semid, 0, IPC_RMID);
    if (status == 0)
    {
        fprintf(stderr, "Removed semaphore set\n");
    }
    else
    {
        fprintf(stderr, "Failed to remove semaphore set\n");
    }

    // int kill(pid_t pid,int sig)
    status = kill(0, SIGKILL);
    if (status == 0)
    {
        fprintf(stderr, "kill successful\n");
    }
    else if (status == -1)
    {
        perror("kill failed\n");
        fprintf(stderr, "Cannot remove semaphore with id=%d\n", semid);
    }
    else
    {
        fprintf(stderr, "kill(2) returned wrong value\n");
    }
}

int main()
{
    key_t semkey;
    int status;

    // /* Structure used for argument to `semop' to describe operations.  */
    // struct sembuf
    // {
    //     unsigned short int sem_num; /* semaphore number */
    //     short int sem_op;           /* semaphore operation */
    //     short int sem_flg;          /* operation flag */
    // };

    Pop.sem_op = -1;
    Pop.sem_flg = SEM_UNDO;

    Vop.sem_op = 1;
    Vop.sem_flg = SEM_UNDO;

    // key_t ftok(const char *__pathname, int __proj_id)
    semkey = ftok("/home/joy/OS/Lab_5/p1/temp/", 1);
    if (semkey == -1)
    {
        perror("ftok() failed");
        exit(1);
    }

    // int semget(key_t __key, int __nsems, int __semflg)
    // semid = semget(semkey, NO_SEM, 0);
    // if (semid == -1)
    // {
    //     perror("semget() failed");
    //     exit(1);
    // }

    while (semget(semkey, NO_SEM, 0) == -1)
        ;

    signal(SIGINT, removeSemaphore);

    int count = 0;

    while (1)
    {
        Pop.sem_num = 3;
        status = P(semid);
        if (status == -1)
        {
            // perror("P error: ");
            exit(0);
        }

        printf("[%3d] I am in p4\n", ++count);

        Vop.sem_num = 1;
        status = V(semid);
        if (status == -1)
        {
            // perror("V error: ");
            exit(0);
        }
    }

    return 0;
}