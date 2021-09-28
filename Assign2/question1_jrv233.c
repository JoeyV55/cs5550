#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void clearBuffer(int *bufferPtr);
bool createdSequence1(int buffer[]);
void *do_work(void *arg);
/*
 Problem 1: Generating a sequence using pthreads
*/

//Create argument struct
struct arguments
{
    pthread_mutex_t *mutex;
    int val;
    int *bufferIndex;
    int *buffer;
    int *count;
    int *totalCount;
    int numCorrect;
};

void printArg(void *arguments)
{
    struct arguments *threadArg = (struct arguments *)arguments;
    printf("Val: %d\n", threadArg->val);
    printf("bufferIndex: %d\n", *(threadArg->bufferIndex));
    printf("bufferVal: %d\n", threadArg->buffer[1]);
    printf("numCorrect: %d\n", threadArg->numCorrect);
}
int main(int argc, char *argv)
{
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    int buffer[3] = {0, 0, 0};
    int bufferIndex = 0;
    int count = 0;
    int totalCount = 0;
    //Make array of three arguments
    int size = 3;
    struct arguments *threadArgs[3];
    int i;
    pthread_mutex_t buffer1Lock;
    pthread_mutex_init(&buffer1Lock, NULL);
    //Initialize arguments struct
    for (i = 0; i < size; i++)
    {
        threadArgs[i] = (struct arguments *)calloc(1, sizeof(struct arguments));
        threadArgs[i]->mutex = &buffer1Lock;
        //Don't use a ++ here! increments i
        threadArgs[i]->val = i + 1;
        threadArgs[i]->buffer = buffer;
        threadArgs[i]->bufferIndex = &bufferIndex;
        threadArgs[i]->count = &count;
        threadArgs[i]->totalCount = &totalCount;
        printArg(threadArgs[i]);
    }

    //Now that we have three sets of arguments, pass them to start the threads.
    if (pthread_create(&thread0, NULL,
                       do_work, (void *)threadArgs[0]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread1, NULL,
                       do_work, (void *)threadArgs[1]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread2, NULL,
                       do_work, (void *)threadArgs[2]))
    {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }

    //Wait for threads to finish with join
    // Join with thread
    if (pthread_join(thread0, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
    if (pthread_join(thread1, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
    // Join with thread
    if (pthread_join(thread2, NULL))
    {
        fprintf(stderr, "Error while joining with child thread\n");
        exit(1);
    }
}

void clearBuffer(int *bufferPtr)
{
    // printf("Val from clear_buffer: %d", bufferPtr[1]);
    int size = 3;
    for (int i = 0; i < size; i++)
    {
        *bufferPtr = 0;
        bufferPtr++;
    }
}

bool createdSequence1(int buffer[])
{
    return (buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3);
}

bool printBuffer(int buffer[])
{
    fprintf(stderr, "\nBuffer: [%d,%d,%d]\n", buffer[0], buffer[1], buffer[2]);
}

//bool finishedSequences(void *lock, void* count){

//}

void *do_work(void *arg)
{
    struct arguments *threadArgs = (struct arguments *)arg;
    int tid = threadArgs->val;
    pthread_mutex_t *mutex = threadArgs->mutex;
    //fprintf(stderr, "Thread with val: %d\n", threadArgs->val);
    int *count = threadArgs->count;
    int *totalCount = threadArgs->totalCount;
    int *indexPtr = NULL;
    bool createdSequence = false;

    //Loop here while shared num sequences is < 10
    //threadArgs->numCorrect < 10
    while (*count < 10)
    {
        //Begin critical section
        pthread_mutex_lock(mutex);
        indexPtr = threadArgs->bufferIndex;
        fprintf(stderr, "My id: %d\n", tid);
        //Set position in buffer.
        threadArgs->buffer[*indexPtr] = threadArgs->val;
        if (*indexPtr == 2)
        {
            //Log this attempt
            *totalCount += 1;
            //printBuffer(threadArgs->buffer);
            createdSequence = createdSequence1(threadArgs->buffer);
            if (createdSequence)
            {
                fprintf(stderr, "123\n");
                *count += 1;
                //fprintf(stderr, "Count: %d\n", *count);
                //If we get here we are done, print the final result.
                if (*count == 10)
                {
                    fprintf(stderr, "Total sequences generated %d\n", *(threadArgs->totalCount));
                    fprintf(stderr, "Number of correct sequences %d\n", *(threadArgs->count));
                }
            }
            *indexPtr = 0;
            clearBuffer(threadArgs->buffer);
        }
        else
        {
            *indexPtr += 1;
        }

        pthread_mutex_unlock(mutex);
        //fprintf(stderr, "Thread %d left crit section with index %d\n", tid, *indexPtr);
        //Sleep for now.
        usleep(200000);
    }

    return NULL;
}

/*
//Critical section
        pthread_mutex_lock(mutex);
        (*count)++;
        fprintf(stderr, "Thread %d entered critical section and made count equal %d\n", threadArgs->val, *count);

        if ((*count) == 2)
        {
            fprintf(stderr, "Two achieved");
        }

        //Set our position
        pthread_mutex_unlock(mutex);
        fprintf(stderr, "Thread %d left critical section\n", threadArgs->val);
        usleep(200000);

        //Check the value of index, do we have a complete sequence?*/