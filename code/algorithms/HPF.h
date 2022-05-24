#ifndef __HPF_H__
#define __HPF_H__
#include "common.h"

void checkForNewHPFProcess(priQueue *hpfProcesses, int msgqID)
{

    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        priQueueInsert(hpfProcesses, newProcess);
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

void finishHPFProcess(priQueue *hpfProcesses)
{
    currentProcess->finishTime = getClk();
    outFinishProcessInfo(currentProcess);
    free(currentProcess);
    currentProcess = NULL;
    currentDeleted = false;
}

void highestPriorityFirst(priQueue *hpfProcesses, int msgqID)
{
    int clk = 0;
    while (1)
    {
        checkForNewHPFProcess(hpfProcesses, msgqID);
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