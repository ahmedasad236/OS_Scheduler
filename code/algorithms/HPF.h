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
        printf("id : %d , arrivalTime : %d , remainingTime : %d , priority : %d , memorySize : %d\n", buff.id, buff.arrivalTime, buff.remainingTime, buff.priority, buff.memorySize);
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, newProcess->memorySize);
        if (!nodeMemory)
        {
            priQueueInsert(readyQueue, newProcess);
        }
        else
        {
            newProcess->memoryNode = nodeMemory;
            priQueueInsert(hpfProcesses, newProcess);
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

void removeFromWaitingList(priQueue *readyQueue, PCB *process)
{
    if (process->prev)
        process->prev->next = process->next;
    if (process->next)
        process->next->prev = process->prev;
    if (process == readyQueue->head)
        readyQueue->head = process->next;
    if (process == readyQueue->tail)
        readyQueue->tail = process->prev;
    process->next = NULL;
    process->prev = NULL;
}
void checkInWaitingList(priQueue *hpfProcesses, priQueue *readyQueue, buddyMemory *memory)
{
    PCB *tempProcess = readyQueue->head;
    while (tempProcess)
    {
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, tempProcess->memorySize);
        if (nodeMemory)
        {
            tempProcess->memoryNode = nodeMemory;
            priQueueInsert(hpfProcesses, createNewProcessP(tempProcess));

            PCB *temp = tempProcess;
            tempProcess = tempProcess->next;
        }
        else
        {
            tempProcess = tempProcess->next;
        }
    }
}

void finishHPFProcess(priQueue *hpfProcesses, priQueue *readyQueue, buddyMemory *memory)
{
    currentProcess->finishTime = getClk();
    outFinishProcessInfo(currentProcess);
    deallocateBuddyMemory(memory, currentProcess->memoryNode);
    free(currentProcess);
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
            finishHPFProcess(hpfProcesses);
        }
    }
}
#endif