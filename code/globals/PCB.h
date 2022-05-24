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
    short priority;
    int memorySize;
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
#endif