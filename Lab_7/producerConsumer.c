#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_QUEUE_SIZE 10

// Circular Queue Structure
typedef struct
{
    int items[MAX_QUEUE_SIZE];
    int front, rear;
    pthread_mutex_t mutex;
    pthread_cond_t notFull, notEmpty;
} CircularQueue;

void initQueue(CircularQueue *q);
void enQueue(CircularQueue *q, int item);
int deQueue(CircularQueue *q);
int isEmptyQ(CircularQueue *q);
int isFullQ(CircularQueue *q);

void *producer(void *data);
void *consumer(void *data);

int main()
{
    int m, n;

    printf("Enter the number of producer threads (m): ");
    scanf("%d", &m);
    printf("Enter the number of consumer threads (n): ");
    scanf("%d", &n);

    CircularQueue q;
    initQueue(&q);

    pthread_t producerThreads[m], consumerThreads[n];

    /* Thread creation --> Thread joining */

    for (int i = 0; i < m; i++)
    {
        // int pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg)
        pthread_create(&producerThreads[i], NULL, producer, (void *)&q);
    }

    for (int i = 0; i < n; i++)
    {
        // int pthread_create(pthread_t *__restrict__ __newthread, const pthread_attr_t *__restrict__ __attr, void *(*__start_routine)(void *), void *__restrict__ __arg)
        pthread_create(&consumerThreads[i], NULL, consumer, (void *)&q);
    }

    for (int i = 0; i < m; i++)
    {
        // int pthread_join(pthread_t __th, void **__thread_return)
        pthread_join(producerThreads[i], NULL);
    }

    for (int i = 0; i < n; i++)
    {
        // int pthread_join(pthread_t __th, void **__thread_return)
        pthread_join(consumerThreads[i], NULL);
    }

    return 0;
}

void initQueue(CircularQueue *q)
{
    q->front = -1;
    q->rear = -1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notFull, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
}

void enQueue(CircularQueue *q, int item)
{
    if (isFullQ(q))
    {
        printf("Queue overflow: Element not added\n");
        return;
    }

    if (isEmptyQ(q))
    {
        q->front = 0;
        q->rear = 0;
    }
    else
    {
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    }

    q->items[q->rear] = item;
}

int deQueue(CircularQueue *q)
{
    if (isEmptyQ(q))
    {
        printf("Queue underflow: Element not removed\n");
        return -1;
    }

    int item = q->items[q->front];

    if (q->front == q->rear)
    {
        q->front = -1;
        q->rear = -1;
    }
    else
    {
        q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    }

    return item;
}

int isEmptyQ(CircularQueue *q)
{
    return q->front == -1;
}

int isFullQ(CircularQueue *q)
{
    return (q->rear + 1) % MAX_QUEUE_SIZE == q->front;
}

void *producer(void *data)
{
    CircularQueue *q = (CircularQueue *)data;
    while (1)
    {

        int item = rand() % 100;
        pthread_mutex_lock(&q->mutex);
        while (isFullQ(q))
        {
            pthread_cond_wait(&q->notFull, &q->mutex);
        }
        enQueue(q, item);

        printf("Produced: %d\n", item);
        pthread_cond_signal(&q->notEmpty);
        pthread_mutex_unlock(&q->mutex);
    }

    return NULL;
}

void *consumer(void *data)
{
    CircularQueue *q = (CircularQueue *)data;
    while (1)
    {

        pthread_mutex_lock(&q->mutex);
        while (isEmptyQ(q))
        {
            pthread_cond_wait(&q->notEmpty, &q->mutex);
        }
        int item = deQueue(q);
        printf("Consumed: %d\n", item);
        pthread_cond_signal(&q->notFull);
        pthread_mutex_unlock(&q->mutex);
    }
    return NULL;
}