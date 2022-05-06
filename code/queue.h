#include "linkedList.h"
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

void queueInsert(PCB *newProcess)
{
    q->size++;
    newProcess->next = q->head;
    newProcess->prev = q->tail;
    if (q->tail == NULL)
        q->head = newProcess;
    else
        q->tail->next = newProcess;
    q->tail = newProcess;
}

void queueInsertPointer(queue *q, PCB *pcb)
{
    queueInsert(q, pcb);
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
    current = q->head;
    while (current != NULL)
    {
        if (current->id == pid)
            return current;
        current = current->next;
    }
    return NULL;
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
