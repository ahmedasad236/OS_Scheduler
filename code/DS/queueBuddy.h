#ifndef __B_QUEUE_H__
#define __B_QUEUE_H__
#include "../globals/PCB.h"
typedef struct PCB PCB;
typedef struct buddyQueue
{
    int size;
    PCB *head;
    PCB *tail;
} buddyQueue;

buddyQueue *createBuddyQueue()
{
    buddyQueue *newQueue = (buddyQueue *)malloc(sizeof(buddyQueue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    return newQueue;
}

void buddyQueueInsert(buddyQueue *q, PCB *newProcess)
{
    q->size++;
    if (!q->head)
    {
        q->head = q->tail = newProcess;
        newProcess->next = NULL;
        return;
    }
    q->tail->next = newProcess;
    q->tail = newProcess;
    newProcess->next = NULL;
}

int buddyQueueSize(buddyQueue *q)
{
    return q->size;
}
bool isEmptyBuddyQueue(buddyQueue *q)
{
    return q->size == 0;
}
void printBuddyQueue(buddyQueue *q)
{
    PCB *temp = q->head;
    while (temp)
    {
        printf("%d ", temp->id);
        temp = temp->next;
    }
    printf("\n");
}
void removeBuddyQueue(buddyQueue *q, PCB *process)
{
    q->size--;
    if (q->size == 0)
    {
        q->head = q->tail = NULL;
        free(process);
        return;
    }
    if (q->head == process)
    {
        q->head = q->head->next;
        free(process);
        return;
    }
    PCB *temp = q->head;
    while (temp->next != process)
    {
        temp = temp->next;
    }
    if (temp->next == q->tail)
    {
        q->tail = temp;
    }
    temp->next = process->next;
    free(process);
}

#endif