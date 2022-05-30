#ifndef __HPF_H__
#define __HPF_H__
#include "common.h"

void checkForNewHPFProcess(int msgqID, priQueue *hpfProcesses, priQueue *readyQueue, buddyMemory *memory)
{
    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 18, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, newProcess->memorySize);
        if (!nodeMemory)
        {
            priQueueInsert(readyQueue, newProcess);
            printf("process with size : %d inserted in ready queue\n", newProcess->memorySize);
        }
        else
        {
            newProcess->memoryNode = nodeMemory;
            priQueueInsert(hpfProcesses, newProcess);
            printf("process with size : %d inserted in running processes\n", newProcess->memorySize);
        }
    }
}

void startCurrentHPFProcess(priQueue *hpfProcesses)
{
    currentProcess = dequeuePriQueue(hpfProcesses);
    if (!currentProcess)
        return;
    int
        clk = getClk();
    startNewProcess(currentProcess);
}


void finishHPFProcess(priQueue *hpfProcesses, priQueue *readyQueue, buddyMemory *memory)
{
    currentProcess->finishTime = getClk();
    outFinishProcessInfo(currentProcess);
    deallocateBuddyMemory(memory, currentProcess->memoryNode);
    printBuddyMemory(memory);
    free(currentProcess);
    checkInWaitingList(hpfProcesses, readyQueue, memory);
    currentProcess = NULL;
    currentDeleted = false;
}

void highestPriorityFirst(int msgqID)
{
    priQueue *hpfProcesses = createPriQueue(compHPFProcesses);
    priQueue *readyQueue = createPriQueue(compHPFProcesses);
    buddyMemory *memory = createBuddyMemory(1024);
    int clk = 0;
    while (1)
    {
        checkForNewHPFProcess(msgqID, hpfProcesses, readyQueue, memory);
        if (!currentProcess)
        {
            startCurrentHPFProcess(hpfProcesses);
        }
        if (currentDeleted)
        {
            finishHPFProcess(hpfProcesses, readyQueue, memory);
        }
    }
}
#endif