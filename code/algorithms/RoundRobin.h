#ifndef __RR_H__
#define __RR_H__
#include "common.h"
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

void checkForNewRoundRobinProcess(int msgqID, queue *runningProcesses)
{
    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 18, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        queueInsert(runningProcesses, newProcess);
        if (!runningProcesses->current)
        {
            runningProcesses->current = runningProcesses->head;
            runNextRoundRobinProcess(runningProcesses);
        }
    }
}
void deleteRoundRobinProcessAndMoveToNextOne(queue *runningProcesses)
{
    runningProcesses->current->finishTime = getClk();
    // outProcessInfo(runningProcesses->current, "finished");
    outFinishProcessInfo(runningProcesses->current);
    deleteCurrentProcess(runningProcesses);
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

void RoundRobin(queue *runningProcesses, int msgqID)
{
    int clk = 0;
    while (1)
    {
        checkForNewRoundRobinProcess(msgqID, runningProcesses);
        if (currentDeleted)
        {
            deleteRoundRobinProcessAndMoveToNextOne(runningProcesses);
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