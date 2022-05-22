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
    printf("old remaining time :%d\n", currentProcess->remainingTime);
    *shm_ptr = -1;
    kill(currentProcess->processID, SIGBUS);
    changeCurrentProcessRemainingTime();
    printf("new remaining time :%d\n", currentProcess->remainingTime);
    if (process->remainingTime < currentProcess->remainingTime)
    {
        outProcessInfo(currentProcess, "stopped");
        *shm_ptr = -1;
        kill(currentProcess->processID, SIGHUP);
        changeCurrentProcessRemainingTime();
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
                                      buff.remainingTime, buff.priority);
        printf("iam here\n");
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
            printf("SRTN Process : %d\n", currentProcess->processID);
            deleteCurrentSRTNProcess();
            runNextSRTNProcess(srtnProcesses);
        }
    }
}
#endif