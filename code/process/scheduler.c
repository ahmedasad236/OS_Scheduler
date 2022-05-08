#include "../globals/global_variables.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../DS/queue.h"
#include "../DS/srtnQueue.h"
#include <string.h>
int cnt = 0;
PCB *currentSrtnProcess = NULL;
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
    // run the process and send remaining time to it
    char *processRunMessage = malloc(sizeof(char) * 100);
    *shm_ptr = -1;
    snprintf(processRunMessage, 100, "gcc process.c -o process_%d && ./process_%d %d %d &", newProcess->id, newProcess->id, newProcess->remainingTime, getpid());
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
void stopProcess(int pid)
{
    kill(pid, SIGUSR1);
    // kill(pid, SIGSTOP);
}

void changeCurrentProcessRemainingTime()
{

    while (*shm_ptr == -1)
        ;
    currentSrtnProcess->remainingTime = *shm_ptr;
}
void newSRTNProcess(srtnQueue *srtnProcesses, PCB *process)
{

    if (isSrtnEmpty(srtnProcesses) && !currentSrtnProcess)
    {
        currentSrtnProcess = process;
        startNewProcess(process);
    }

    else
    {

        // send signal to current process to get remaining time
        *shm_ptr = -1;
        kill(currentSrtnProcess->processID, SIGUSR1);
        changeCurrentProcessRemainingTime();

        if (process->remainingTime < currentSrtnProcess->remainingTime)
        {
            *shm_ptr = -1;
            stopProcess(currentSrtnProcess->processID);
            changeCurrentProcessRemainingTime();
            srtnQueueInsert(srtnProcesses, currentSrtnProcess);
            currentSrtnProcess = process;
            startNewProcess(process);
        }

        else
            srtnQueueInsert(srtnProcesses, process);
    }
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
        // *shm_ptr = current->processID * -1;
        continueProcess(current->processID);
        // printf("continued : %d\n", current->processID);
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
        stopProcess(current->processID);
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

int getShmKey()
{
    int key = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = shmat(key, NULL, 0);
    return key;
}

void deleteCurrentSRTNProcess()
{
    if (currentSrtnProcess)
        free(currentSrtnProcess);
    currentDeleted = false;
}

void runNextSRTNProcess(srtnQueue *srtnProcesses)
{
    currentSrtnProcess = dequeueSrtnQueue(srtnProcesses);
    if (!currentSrtnProcess)
        return;
    if (currentSrtnProcess->processID == -1)
    {
        startNewProcess(currentSrtnProcess);
        cnt++;
    }
    else
    {
        // *shm_ptr = current->processID * -1;
        continueProcess(currentSrtnProcess->processID);
        // printf("continued : %d\n", current->processID);
    }
}

void checkForNewSRTNProcess(srtnQueue *srtnProcesses, int msgqID)
{
    struct processBuff buff;
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority);

        printf("new process remaining time : %d\n", newProcess->remainingTime);
        printf("size : %d\n", srtnProcesses->size);
        newSRTNProcess(srtnProcesses, newProcess);
    }
}

void shortestRemainingTimeNext(srtnQueue *srtnProcesses, int msgqID)
{
    while (1)
    {
        checkForNewSRTNProcess(srtnProcesses, msgqID);
        if (currentDeleted)
        {
            printf("SRTN Process : %d\n", currentSrtnProcess->processID);
            deleteCurrentSRTNProcess();
            runNextSRTNProcess(srtnProcesses);
        }
    }
}
int main(int argc, char *argv[])
{
    queue *runningProcesses = createQueue();
    srtnQueue *srtnProcesses = createSrtnQueue();
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
        break;
    case SRTN:
        shortestRemainingTimeNext(srtnProcesses, msgq_processGenerator_id);
        break;
    case RR:
        RoundRobin(runningProcesses, msgq_processGenerator_id);
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
