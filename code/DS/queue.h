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

void queueDeleteFirst(queue *q)
{
    if (q->head == NULL)
    {
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
    PCB *cur = q->current;
    if (!cur)
        cur = q->head;
    int i;
    for (i = 0; i < s; i++)
    {
        printf("%d ", cur->id);
        cur = cur->next;
    }
}
#endif