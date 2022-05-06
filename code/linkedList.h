#include "PCB.h"
int processTableLength = 0;
PCB *head = NULL;
PCB *tail = NULL;
PCB *current = NULL;

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
        printf("%d %d\n", current->id , current->arrivalTime);
        current = current->next;
    };
}
