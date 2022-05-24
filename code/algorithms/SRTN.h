#ifndef __SRTN_H__
#define __SRTN_H__
void newSRTNProcess(priQueue *srtnProcesses, PCB *process)
{
    if (isPriorityQueueEmpty(srtnProcesses) && !currentProcess)
    {
        currentProcess = process;
        startNewProcess(process);
        return;
    }
    *shm_ptr = -1;
    kill(currentProcess->processID, SIGBUS);
    changeCurrentProcessRemainingTime();
    if (process->remainingTime < currentProcess->remainingTime)
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
void deleteCurrentSRTNProcess()
{
    if (currentProcess)
    {
        currentProcess->remainingTime = 0;
        currentProcess->finishTime = getClk();
        outFinishProcessInfo(currentProcess);
        free(currentProcess);
        currentProcess = NULL;
    }
    currentDeleted = false;
}

void runNextSRTNProcess(priQueue *srtnProcesses)
{
    currentProcess = dequeuePriQueue(srtnProcesses);
    if (!currentProcess)
        return;
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

void checkForNewSRTNProcess(priQueue *srtnProcesses, int msgqID)
{
    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority, buff.memorySize);
        newSRTNProcess(srtnProcesses, newProcess);
    }
}

void shortestRemainingTimeNext(priQueue *srtnProcesses, int msgqID)
{
    while (1)
    {
        checkForNewSRTNProcess(srtnProcesses, msgqID);
        if (currentDeleted)
        {
            deleteCurrentSRTNProcess();
            runNextSRTNProcess(srtnProcesses);
        }
    }
}
#endif