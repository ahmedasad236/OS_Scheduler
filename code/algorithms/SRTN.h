#ifndef __SRTN_H__
#define __SRTN_H__
#include "common.h"
void newSRTNProcess(priQueue *srtnProcesses, PCB *process)
{
    if (isPriorityQueueEmpty(srtnProcesses) && !currentProcess)
    {
        currentProcess = process;
        startNewProcess(process);
        return;
    }
    if (!currentDeleted)
    {
        *shm_ptr = -1;
        kill(currentProcess->processID, SIGBUS);
        changeCurrentProcessRemainingTime();
    }
    if (currentDeleted || (process->remainingTime < currentProcess->remainingTime))
    {
        outProcessInfo(currentProcess, "stopped");
        kill(currentProcess->processID, SIGSTOP);
        priQueueInsert(srtnProcesses, currentProcess);
        currentProcess = process;
        startNewProcess(process);
    }

    else
        priQueueInsert(srtnProcesses, process);
}
void deleteCurrentSRTNProcess(priQueue *srtnProcesses, priQueue *readyQueue, buddyMemory *memory)
{
    if (currentProcess)
    {
        currentProcess->remainingTime = 0;
        currentProcess->finishTime = getClk();
        outFinishProcessInfo(currentProcess);
        outDeallocateMemory(memory, currentProcess);
        deallocateBuddyMemory(memory, currentProcess->memoryNode);
        free(currentProcess);
        checkInWaitingList(srtnProcesses, readyQueue, memory);
        currentProcess = NULL;
    }
    currentDeleted = false;
}

void runNextSRTNProcess(priQueue *srtnProcesses)
{
    currentProcess = dequeuePriQueue(srtnProcesses);
    if (!currentProcess)
    {
        printf("Not current process\n");
        return;
    }
    if (currentProcess->processID == -1)
    {
        startNewProcess(currentProcess);
        cnt++;
    }
    else
    {
        continueProcess(currentProcess);
    }
}

void checkForNewSRTNProcess(int msgqID, priQueue *srtnProcesses, priQueue *readyQueue, buddyMemory *memory)
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
            printf("process with size : %d inserted in running processes\n", newProcess->memorySize);
            newSRTNProcess(srtnProcesses, newProcess);
            outAllocateMemory(memory, newProcess);
        }
    }
}

void shortestRemainingTimeNext(int msgqID)
{
    priQueue *srtnProcesses = createPriQueue(compSRTNProcesses);
    priQueue *readyQueue = createPriQueue(compSRTNProcesses);
    buddyMemory *memory = createBuddyMemory(1024);
    while (1)
    {
        checkForNewSRTNProcess(msgqID, srtnProcesses, readyQueue, memory);
        if (currentDeleted)
        {
            deleteCurrentSRTNProcess(srtnProcesses, readyQueue, memory);
            runNextSRTNProcess(srtnProcesses);
        }
    }
}
#endif