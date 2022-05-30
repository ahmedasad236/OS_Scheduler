#ifndef __RR_H__
#define __RR_H__
#include "common.h"
#include "../DS/queueBuddy.h"
void runNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    if (!current)
        return;
    if (current->processID == -1)
    {
        startNewProcess(current);
        cnt++;
    }
    else
    {
        continueProcess(current);
    }
}

void checkForNewRoundRobinProcess(int msgqID, queue *runningProcesses, buddyQueue *readyQueue, buddyMemory *memory)
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
            buddyQueueInsert(readyQueue, newProcess);
        }
        else
        {
            newProcess->memoryNode = nodeMemory;
            queueInsert(runningProcesses, newProcess);
            outAllocateMemory(memory, newProcess);
        }
        if (!runningProcesses->current)
        {
            runningProcesses->current = runningProcesses->head;
            runNextRoundRobinProcess(runningProcesses);
        }
    }
}
void checkInWaitingListRR(queue *runningProcesses, buddyQueue *readyQueue, buddyMemory *memory)
{
    PCB *tempProcess = readyQueue->head;
    int i = 0;
    printBuddyQueue(readyQueue);
    while (tempProcess)
    {
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, tempProcess->memorySize);
        if (nodeMemory)
        {
            tempProcess->memoryNode = nodeMemory;
            queueInsert(runningProcesses, createNewProcessP(tempProcess));
            outAllocateMemory(memory, tempProcess);
            PCB *temp = tempProcess;
            tempProcess = tempProcess->next;
            removeBuddyQueue(readyQueue, temp);
            printBuddyQueue(readyQueue);
            if (!runningProcesses->current)
                runningProcesses->current = runningProcesses->head;
        }
        else
        {
            tempProcess = tempProcess->next;
        }
    }
}
void deleteRoundRobinProcessAndMoveToNextOne(queue *runningProcesses, buddyQueue *readyQueue, buddyMemory *memory)
{
    outFinishProcessInfo(runningProcesses->current);
    outDeallocateMemory(memory, runningProcesses->current);
    deallocateBuddyMemory(memory, runningProcesses->current->memoryNode);
    printBuddyMemory(memory);
    deleteCurrentProcess(runningProcesses);
    checkInWaitingListRR(runningProcesses, readyQueue, memory);
    runNextRoundRobinProcess(runningProcesses);
    currentDeleted = false;
}

void moveToNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    if (current == NULL || current->next == current)
        return;
    // send signal to the current process to stop it
    if (current->processID != -1)
        stopProcess(current, SIGUSR1);
    runningProcesses->current = current->next;
    runNextRoundRobinProcess(runningProcesses);
}

void RoundRobin(int msgqID)
{

    queue *runningProcesses = createQueue();
    buddyQueue *readyQueue = createBuddyQueue();
    buddyMemory *memory = createBuddyMemory(1024);
    int clk = 0;
    while (1)
    {
        checkForNewRoundRobinProcess(msgqID, runningProcesses, readyQueue, memory);
        if (currentDeleted)
        {
            deleteRoundRobinProcessAndMoveToNextOne(runningProcesses, readyQueue, memory);
            clk = getClk();
        }
        else if (getClk() >= clk + time_slot)
        {
            moveToNextRoundRobinProcess(runningProcesses);
            clk = getClk();
        }
    }
}
#endif