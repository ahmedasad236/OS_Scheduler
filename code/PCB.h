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
    struct PCB *next;
    struct PCB *prev;
} PCB;

PCB *createNewProcess(int id, int arrivalTime, int remainingTime, short priority)
{
    PCB *newProcess = (PCB *)malloc(sizeof(PCB));
    newProcess->id = id;
    newProcess->arrivalTime = arrivalTime;
    newProcess->remainingTime = remainingTime;
    newProcess->priority = priority;
    newProcess->state = 0;
    newProcess->next = NULL;
    newProcess->prev = NULL;
    newProcess->processID = -1;
    return newProcess;
}