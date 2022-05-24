#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "../globals/PCB.h"
typedef struct PCB PCB;
typedef struct queue
{
    int size;
    PCB *head;
    PCB *tail;
    PCB *current;
} queue;

queue *createQueue()
{
    queue *newQueue = (queue *)malloc(sizeof(queue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->current = NULL;
    return newQueue;
}

void queueInsert(queue *q, PCB *newProcess)
{
    q->size++;
    if (q->head == NULL)
    {
        q->head = q->tail = newProcess;
        newProcess->next = newProcess->prev = newProcess;
        return;
    }
    q->head->prev = newProcess;
    q->tail->next = newProcess;
    newProcess->next = q->head;
    newProcess->prev = q->tail;
    q->tail = newProcess;
}
void insertNewProcess(queue *q, PCB *newProcess)
{
    queueInsert(q, newProcess);
    printf("i reached after inserting\n");
    // if (q->current == NULL)
    // {
    //     queueInsert(q, newProcess);
    //     return;
    // }
    // q->size++;
    // newProcess->next = q->current;
    // newProcess->prev = q->current->prev;
    // q->current->prev->next = newProcess;
    // q->current->prev = newProcess;
    // printf(":: %p %p %p", newProcess, newProcess->prev, newProcess->next);
}

void queueDeleteFirst(queue *q)
{
    if (q->head == NULL)
    {
        perror("No process in the queue");
        return;
    }
    q->current = q->head;
    q->head = q->head->next;
    free(q->current);
    q->size--;
}

PCB *queueFind(queue *q, int pid)
{
    PCB *current = q->head;
    while (current != NULL)
    {
        if (current->id == pid)
            return current;
        current = current->next;
    }
    return NULL;
}

void deleteCurrentProcess(queue *q)
{
    if (q->current == NULL)
    {
        perror("No process in the queue");
        return;
    }
    q->size--;
    if (q->size == 0)
    {
        free(q->current);
        q->head = q->tail = q->current = NULL;
        return;
    }
    q->current->prev->next = q->current->next;
    q->current->next->prev = q->current->prev;
    PCB *temp = q->current;
    q->current = q->current->next;
    free(temp);
}
void deleteProcess(queue *q, int pid)
{
    PCB *process = queueFind(q, pid);
    if (process == NULL)
    {
        printf("No process found");
        return;
    }
    if (process->prev == NULL)
    {
        q->head = q->tail = q->current = NULL; // prev = null => one element in the queue
        free(process);
    }
    else
    {
        process->prev->next = process->next;
        process->next->prev = process->prev;
        free(process);
    }
}

bool isEmpty(queue *q)
{
    return q->size == 0;
}
void printQueue(queue *q)
{
    int s = q->size;
    printf("size : %d\n", s);
    PCB *cur = q->current;
    if (!cur)
        cur = q->head;
    int i;
    for (i = 0; i < s; i++)
    {
        printf("%d ", cur->id);
        if (q->current == NULL)
        {
            printf("NULL wrong\n");
            break;
        }
        cur = cur->next;
    }
}
#endif