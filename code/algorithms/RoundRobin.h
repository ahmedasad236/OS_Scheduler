#ifndef __RR_H__
#define __RR_H__
#include "common.h"
void runNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    if (!current)
        return;
    if (current->id == 0)
        printTest("id 0");
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

void checkForNewRoundRobinProcess(queue *runningProcesses, queue *readyQueue, buddyMemory *memory, int msgqID)
{
    struct processBuff buff;
    // printf("checking............\n");
    while (msgrcv(msgqID, &buff, 24, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        printf("id : %d , arrival : %d , remaining : %d , priority : %d , memorySize : %d\n", newProcess->id, newProcess->arrivalTime, newProcess->remainingTime, newProcess->priority, newProcess->memorySize);
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, newProcess->memorySize);
        // printBuddyMemory(memory);
        if (nodeMemory)
        {
            newProcess->memory = nodeMemory;
            queueInsert(runningProcesses, newProcess);
        }
        else
        {
            queueInsert(readyQueue, newProcess);
        }
        if (!runningProcesses->current)
        {
            runningProcesses->current = runningProcesses->head;
            runNextRoundRobinProcess(runningProcesses);
        }
    }
}
void removeFromWaitingListRR(PCB *process)
{
    if (process->prev)
        process->prev->next = process->next;
    if (process->next)
        process->next->prev = process->prev;
    free(process);
}

void checkInWaitingListRR(queue *runningProcesses, queue *readyQueue, buddyMemory *memory)
{
    PCB *tempProcess = readyQueue->head;
    while (tempProcess)
    {
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, tempProcess->memorySize);
        if (nodeMemory)
        {
            printTest(itoa(tempProcess->id));
            tempProcess->memory = nodeMemory;
            queueInsert(runningProcesses, tempProcess);
            if (!runningProcesses->current)
            {
                printTest("current is null");
                runningProcesses->current = runningProcesses->head;
            }
            else
            {
                printTest("current is not null");
            }
            removeFromWaitingListRR(tempProcess);
            return;
        }
        tempProcess = tempProcess->next;
    }
}

void deleteRoundRobinProcessAndMoveToNextOne(buddyMemory *root, queue *runningProcesses, queue *readyQueue)
{
    runningProcesses->current->finishTime = getClk();
    // outProcessInfo(runningProcesses->current, "finished");
    outFinishProcessInfo(runningProcesses->current);
    deallocateBuddyMemory(root, runningProcesses->current->memory);
    deleteCurrentProcess(runningProcesses);
    checkInWaitingListRR(runningProcesses, readyQueue, root);
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
    if (current->id == 0)
        printTest("current id is 0");
    runningProcesses->current = current->next;
    runNextRoundRobinProcess(runningProcesses);
}
void RoundRobin(queue *runningProcesses, int msgqID)
{
    int clk = 0;
    queue *readyQueue = createQueue();
    buddyMemory *root = createBuddyMemory(1024);
    printf("Round Robin\n");
    while (1)
    {
        checkForNewRoundRobinProcess(runningProcesses, readyQueue, root, msgqID);
        if (currentDeleted)
        {
            deleteRoundRobinProcessAndMoveToNextOne(root, runningProcesses, readyQueue);
            // checkInWaitingListRR(runningProcesses, readyQueue, root);
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