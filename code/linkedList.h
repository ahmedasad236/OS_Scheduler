#include "headers.h"
typedef struct PCB
{
    int processID;
    int id;
    int arrivalTime;
    int remainingTime;
    int startTime;
    int finishTime;
    short priority;
    bool state; // 0=>ready 1=>running
    PCB *next;
    PCB *prev;
}PCB;

int processTableLength = 0;
PCB *head = NULL;
PCB *tail = NULL;
PCB *current = NULL;

 PCB *createNewProcess(int id, int arrivalTime, int remainingTime, short priority)
{
     PCB *newProcess = ( PCB *)malloc(sizeof( PCB));
    newProcess->id = id;
    newProcess->arrivalTime = arrivalTime;
    newProcess->remainingTime = remainingTime;
    newProcess->priority = priority;
    newProcess->state = 0;
    newProcess->next = NULL;
    newProcess->prev = NULL;
    return newProcess;
}

void insertLast(PCB *newProcess)
{
    processTableLength++;
    newProcess->next = NULL;
    newProcess->prev = tail;
    if (tail == NULL)
        head = newProcess;
    else
        tail->next = newProcess;
    tail = newProcess;
}

PCB *find(int pid)
{
    current = head;
    while (current != NULL)
    {
        if (current->id == pid)
            return current;
        current = current->next;
    }
    return NULL;
}
void deleteFirst()
{
    if (head == NULL)
        return;
    current = head;
    head = head->next;
    if (head != NULL)
        head->prev = NULL;
    processTableLength--;
    free(current);
}

void printLinkList()
{
    current = head;
    while (current != NULL)
    {
        printf("%d\n", current->id);
        current = current->next;
    };
}
