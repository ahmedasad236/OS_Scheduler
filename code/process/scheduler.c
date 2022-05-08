#include "../globals/global_variables.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../DS/queue.h"
#include <string.h>
#include "../DS/priorityQueue.h"
int cnt = 0;
PCB *currentProcess = NULL;
int *shm_ptr = NULL;
bool currentDeleted = false;

typedef struct ALGORITHM_TYPE
{
    long mtype;
    int algoType;

} ALGORITHM_TYPE;
struct processBuff
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    long mtype;
};

void startNewProcess(PCB *newProcess)
{
    char *processRunMessage = malloc(sizeof(char) * 100);
    *shm_ptr = -1;
    snprintf(processRunMessage, 100, "gcc process.c -o process__%d && ./process__%d %d %d &", newProcess->id, newProcess->id, newProcess->remainingTime, getpid());
    printf("%s\n", processRunMessage);
    system(processRunMessage);
    while (*shm_ptr == -1)
        ;
    newProcess->processID = *shm_ptr;
}
void SIGUSR1_handler(int sig)
{
    currentDeleted = true;
    signal(SIGUSR1, SIGUSR1_handler);
}

void continueProcess(int pid)
{
    printf("process %d is running\n", pid);
    kill(pid, SIGCONT);
    kill(pid, SIGUSR2);
}

void changeCurrentProcessRemainingTime()
{

    while (*shm_ptr == -1)
        ;
    currentProcess->remainingTime = *shm_ptr;
}
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
        continueProcess(current->processID);
    }
}

void checkForNewRoundRobinProcess(int msgqID, queue *runningProcesses)
{
    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority);
        insertNewProcess(runningProcesses, newProcess);
        if (!runningProcesses->current)
            runningProcesses->current = runningProcesses->head;
    }
}

void moveToNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    if (current == NULL || current->next == current)
        return;
    if (current->processID == -1 && current == current->next)
        printf("only one process\n");
    // send signal to the current process to stop it
    if (current->processID != -1)
    {
        // stopProcess(current->processID);
        kill(current->processID, SIGUSR1);
    }
    runningProcesses->current = current->next;
    runNextRoundRobinProcess(runningProcesses);
}

char *itoa(int num)
{
    int length = snprintf(NULL, 0, "%d", num);
    char *str = malloc(length + 1);
    snprintf(str, length + 1, "%d", num);
    return str;
}

void deleteRoundRobinProcessAndMoveToNextOne(queue *runningProcesses)
{
    deleteCurrentProcess(runningProcesses);
    runNextRoundRobinProcess(runningProcesses);
    currentDeleted = false;
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
////////////////////////////////////////////////////////////////////////////////////

int getShmKey()
{
    int key = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = shmat(key, NULL, 0);
    return key;
}

void deleteCurrentSRTNProcess()
{
    if (currentProcess)
    {
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
        printf("1\n");
        startNewProcess(currentProcess);
        cnt++;
    }
    else
    {
        printf("2\n");
        continueProcess(currentProcess->processID);
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

void checkForNewHPFProcess(priQueue *hpfProcesses, int msgqID)
{

    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority);

        priQueueInsert(hpfProcesses, newProcess);
    }
}

void startCurrentHPFProcess(priQueue *hpfProcesses)
{
    currentProcess = dequeuePriQueue(hpfProcesses);
    if (!currentProcess)
        return;
    int clk = getClk();
    currentProcess->startTime = clk;
    currentProcess->waitingTime = clk - currentProcess->arrivalTime;
    startNewProcess(currentProcess);
}

void finishHPFProcess(priQueue *hpfProcesses)
{
    currentProcess->finishTime = getClk();
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

int main(int argc, char *argv[])
{
    signal(SIGUSR1, SIGUSR1_handler);
    int key = getShmKey();
    initClk();
    int msgq_processGenerator_id;
    do
    {
        msgq_processGenerator_id = msgget(88, 0666 | IPC_CREAT);
    } while (msgq_processGenerator_id == -1);
    struct processBuff processTemp;

    ALGORITHM_TYPE algorithm;
    algorithm.mtype = 120;
    msgrcv(msgq_processGenerator_id, &algorithm, 4, 120, !IPC_NOWAIT);
    int msgrcv_val;
    switch (algorithm.algoType)
    {
    case HPF:
        highestPriorityFirst(createPriQueue(compHPFProcesses), msgq_processGenerator_id);
        break;
    case SRTN:
        shortestRemainingTimeNext(createPriQueue(compSRTNProcesses), msgq_processGenerator_id);
        break;
    case RR:
        RoundRobin(createQueue(), msgq_processGenerator_id);
        break;
    default:
        // perror("some thing wrong in chosen algorithms");
        break;
    }

    shmctl(key, IPC_RMID, (struct shmid_ds *)0);
    // upon termination release the clock resources.
    destroyClk(true);

    return 0;
}
