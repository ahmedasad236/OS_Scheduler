#include "global_variables.h"
#include "PCB.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "queue.h"
// #include "srtnQueue.h"
#include <string.h>
PCB *currentSrtnProcess;
int *shm_ptr;
// srtnQueue *srtnProcesses;
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
    printf("ID : %d\n", *shm_ptr);
    newProcess->processID = *shm_ptr;
}
void SIGUSR1_handler(int sig)
{
    currentDeleted = true;
    signal(SIGUSR1, SIGUSR1_handler);
}

void runNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    if (current->processID == -1)
        startNewProcess(current);
    else
        kill(current->processID, SIGCONT);
}
void checkForNewRoundRobinProcess(int msgqID, queue *runningProcesses)
{
    struct processBuff buff;
    printf("1\n");
    while (msgrcv(msgqID, &buff, 14, 0, IPC_NOWAIT) != -1)
    {
        PCB *newProcess;
        newProcess = createNewProcess(buff.id, buff.arrivalTime,
                                      buff.remainingTime, buff.priority);
        insertNewProcess(runningProcesses, newProcess);
        if (!runningProcesses->current)
            runningProcesses->current = runningProcesses->head;
    }
    // printQueue(runningProcesses);
}
void moveToNextRoundRobinProcess(queue *runningProcesses)
{
    PCB *current = runningProcesses->current;
    printf("2\n");
    if (current == NULL || current->next == NULL)
    {
        printf("%p ", current);
        printf("no you aren't");
        return;
    }
    printf("finally you reached");
    sleep(1);
    kill(current->processID, SIGSTOP);
    current = current->next;
    // send signal to the current process to stop it
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
    int clk = getClk();
    while (1)
    {
        checkForNewRoundRobinProcess(msgqID, runningProcesses);
        // printf("clk : %d , clk : %d\n", clk, getClk());
        if (currentDeleted)
        {
            printf("iam here and this is wrong");
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

void getShmKey()
{
    int key = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = shmat(key, NULL, 0);
}
int main(int argc, char *argv[])
{
    queue *runningProcesses = createQueue();
    // srtnProcesses = createSrtnQueue();
    signal(SIGUSR1, SIGUSR1_handler);
    getShmKey();
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
    printf("%d\n", algorithm.algoType);
    printf("%d\n", msgq_processGenerator_id);
    int msgrcv_val;
    switch (algorithm.algoType)
    {
    case HPF:
        break;
    case SRTN:
        break;
    case RR:
        RoundRobin(runningProcesses, msgq_processGenerator_id);
        break;
    default:
        // perror("some thing wrong in chosen algorithms");
        break;
    }
    // upon termination release the clock resources.
    destroyClk(true);
}
// void shortestRemainingTimeNext(PCB *process)
// {

//     if (isSrtnEmpty(srtnProcesses))
//     {
//         currentSrtnProcess = process;
//         startNewProcess(process);
//     }

//     else
//     {

//         if (process->remainingTime < currentSrtnProcess->remainingTime)
//         {
//             kill(currentSrtnProcess->processID, SIGINT);
//             srtnQueueInsert(srtnProcesses, currentSrtnProcess);
//             currentSrtnProcess = process;
//             startNewProcess(process);
//         }
//         else
//             srtnQueueInsert(srtnProcesses, process);
//     }
// }