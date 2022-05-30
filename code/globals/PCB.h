#ifndef __PCB_H__
#define __PCB_H__
#include "headers.h"
typedef struct PCB
{
    int processID;
    int id;
    int arrivalTime;
    int remainingTime;
    int totalRunTime;
    int startTime;
    int finishTime;
    int waitingTime;
    int memorySize;
    short priority;
    buddyMemory *memoryNode;
    bool state; // 0=>ready 1=>running
    struct PCB *next;
    struct PCB *prev;
} PCB;

PCB *createNewProcess(int id, int arrivalTime, int runTime, short priority, int memorySize)
{
    PCB *newProcess = (PCB *)malloc(sizeof(PCB));
    newProcess->id = id;
    newProcess->arrivalTime = arrivalTime;
    newProcess->remainingTime = runTime;
    newProcess->priority = priority;
    newProcess->totalRunTime = runTime;
    newProcess->state = 0;
    newProcess->next = NULL;
    newProcess->prev = NULL;
    newProcess->processID = -1;
    newProcess->memorySize = memorySize;
    return newProcess;
}
PCB *createNewProcessP(PCB *p)
{
    PCB *newProcess = (PCB *)malloc(sizeof(PCB));
    newProcess->id = p->id;
    newProcess->arrivalTime = p->arrivalTime;
    newProcess->remainingTime = p->remainingTime;
    newProcess->priority = p->priority;
    newProcess->totalRunTime = p->totalRunTime;
    newProcess->state = p->state;
    newProcess->next = NULL;
    newProcess->prev = NULL;
    newProcess->processID = p->processID;
    newProcess->memorySize = p->memorySize;
    newProcess->memoryNode = p->memoryNode;
    return newProcess;
}
#endif