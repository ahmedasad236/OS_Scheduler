#include "linkedList.h"
typedef struct PCB PCB;
typedef struct srtnQueue
{
    int size = 0;
    PCB *head = NULL;
    PCB *tail = NULL;
    PCB *top = NULL;
} srtnQqueue;

srtnQueue *createSrtnQueue()
{
    srtnQueue *newQueue = (srtnQueue *)malloc(sizeof(srtnQueue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->top = NULL;
    return newQueue;
}

void linkTwoPointers(PCB* &p1, PCB* &p2) {
    p2->next = p1->next;
    p1->next->prev = p2;
    p1->next = p2;
    p2->prev = p1;
}

void srtnQueueInsert(PCB *newProcess)
{
    
    q->size++;
    
    if(!head){
        top = head = newProcess;
        newProcess->next = tail = NULL;
        return;
    }
    
    PCB* tempPtr = head;
    while(tempPtr->next && remainingTime >= tempPtr.remainingTime) tempPtr = tempPtr->next;
    
    
    if(tempPtr == tail){
        if(remainingTime >= tail.remainingTime){
            tail->next = newProcess;
            newProcess->prev = tail;
            tail = newProcess;
        }
        else
            linkTwoPointers(tail->prev, newProcess);
    }

    else 
        linkTwoPointers(tempPtr->prev, newProcess);
}


void srtnQueueDeleteFirst(srtnQueue*q)
{
    if (q->head == NULL)
    {
        perror("No process in the queue");
        return;
    }
    q->top = q->head;
    q->head = q->head->next;
    free(q->top);
    q->size--;
}

PCB* peek(){
    return head;
}


PCB *srtnQueueFind(srtnQueue *q, int pid)
{
    top = q->head;
    while (top != NULL)
    {
        if (top->id == pid)
            return top;
        top = top->next;
    }
    return NULL;
}

bool isEmpty(srtnQueue*q)
{
    return q->size == 0;
}
