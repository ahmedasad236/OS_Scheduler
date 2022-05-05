#include "headers.h"
struct PCB
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    bool state; // 0=>ready 1=>running
    struct PCB *next;
    struct PCB *prev;
};

int processTableLength = 0;
struct PCB *head = NULL;
struct PCB *tail = NULL;
struct PCB *current = NULL;

void insertLast(int id,int arrivalTime,int remainingTime,short priority)
{
    struct PCB *newProcess = (struct PCB*) malloc(sizeof(struct PCB));
    newProcess->id = id;
    newProcess->arrivalTime = arrivalTime;
    newProcess->priority = priority;
    newProcess->remainingTime = remainingTime;
    newProcess->state = 0;
    processTableLength++;

    newProcess->next = NULL;
    newProcess->prev = tail;
    if(tail == NULL)
        head = newProcess;
    else
        tail->next = newProcess;
    tail = newProcess;
}

struct PCB* find(int pid)
{
    current = head;
    while(current != NULL)
    {
        if(current->id == pid)
            return current;
        current = current->next;
    }
    return NULL;
}
void deleteFirst()
{
    if(head == NULL)
        return;
    current = head;
    head = head->next;
    if(head != NULL)
        head->prev = NULL;
    processTableLength--;
    free(current);
}

void printLinkList()
{
    current = head;
    while(current != NULL)
    {
        printf("%d\n",current->id);
        current = current->next;
    };
}