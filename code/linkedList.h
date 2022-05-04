#include "headers.h"
typedef struct PCB
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    bool state; // 0=>ready 1=>running
    PCB *next;
    PCB *prev;
} PCB;
int processTableLength = 0;
PCB *head = NULL;
PCB *tail = NULL;
PCB *current = NULL;

void insertLast(int id,int arrivalTime,int remainingTime,short priority)
{
    PCB *newProcess = (PCB*) malloc(sizeof(PCB));
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

PCB* find(int pid)
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
bool deleteFirst()
{
    if(head == NULL)
        return 0;
    current = head;
    head = head->next;
    // delete current;
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