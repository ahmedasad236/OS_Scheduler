#ifndef __HPF_H__
#define __HPF_H__
#include "common.h"

void checkForNewHPFProcess(priQueue *hpfProcesses, priQueue *waitingList, buddyMemory *memory, int msgqID)
{

    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        buddyMemory *processMemory = insertBuddyMemoryProcess(memory, newProcess->memorySize);
        if (!processMemory)
        {
            priQueueInsert(waitingList, newProcess);
        }
        else
        {
            newProcess->memory = processMemory;
            priQueueInsert(hpfProcesses, newProcess);
        }
    }
}

void startCurrentHPFProcess(priQueue *hpfProcesses)
{
    currentProcess = dequeuePriQueue(hpfProcesses);
    if (!currentProcess)
        return;
    int clk = getClk();
    startNewProcess(currentProcess);
}
void finishHPFProcess(priQueue *hpfProcesses, buddyMemory *memory)
{
    currentProcess->finishTime = getClk();
    outFinishProcessInfo(currentProcess);
    deallocateBuddyMemory(memory, currentProcess->memory);
    free(currentProcess);
    currentProcess = NULL;
    currentDeleted = false;
}
void checkInWaitingListHPF(priQueue *hpfProcesses, priQueue *waitingList, buddyMemory *memory)
{
    PCB *tempProcess = waitingList->head;
    while (tempProcess)
    {
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, tempProcess->memorySize);
        if (nodeMemory)
        {
            tempProcess->memory = nodeMemory;
            priQueueInsert(hpfProcesses, tempProcess);
            return;
        }
        tempProcess = tempProcess->next;
    }
}

void highestPriorityFirst(priQueue *hpfProcesses, int msgqID)
{
    int clk = 0;
    priQueue *waitingList = createPriQueue(compHPFProcesses);
    buddyMemory *memory = createBuddyMemory(1024);
    while (1)
    {
        checkForNewHPFProcess(waitingList, hpfProcesses, memory, msgqID);
        if (!currentProcess)
        {
            startCurrentHPFProcess(hpfProcesses);
        }
        if (currentDeleted)
        {
            finishHPFProcess(hpfProcesses , memory);
            checkInWaitingListHPF(hpfProcesses, waitingList, memory);
        }
    }
}
#endif