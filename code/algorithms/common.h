#ifndef __COMMON_H__
#define __COMMON_H__
#include "../globals/global_variables.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../DS/queue.h"
#include <string.h>
#include "../DS/priorityQueue.h"
#include "../globals/headers.h"
int cnt = 0;
PCB *currentProcess = NULL;
int *shm_ptr = NULL;
bool currentDeleted = false;
// FILE *outFile = NULL;
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
    int memorySize;
    short priority;
    long mtype;
};
void outProcessInfo(PCB *p, char *state)
{
    FILE *outFile = fopen("process_info.txt", "a");
    fprintf(outFile, "At time %d process %d %s arr : %d total : %d remain : %d wait : %d\n", getClk(), p->id, state, p->arrivalTime, p->totalRunTime, p->remainingTime, getClk() - p->arrivalTime - p->totalRunTime + p->remainingTime);
    fclose(outFile);
}
void startNewProcess(PCB *newProcess)
{
    newProcess->waitingTime = getClk() - newProcess->arrivalTime;
    char *processRunMessage = (char *)malloc(sizeof(char) * 100);
    *shm_ptr = -1;
    snprintf(processRunMessage, 100, "gcc process.c -o process__%d && ./process__%d %d %d &", newProcess->id, newProcess->id, newProcess->remainingTime, getpid());
    system(processRunMessage);
    while (*shm_ptr == -1)
        ;
    newProcess->processID = *shm_ptr;
    outProcessInfo(newProcess, "started");
}
void SIGUSR1_handler(int sig)
{
    currentDeleted = true;
    signal(SIGUSR1, SIGUSR1_handler);
}
void continueProcess(PCB *p)
{
    outProcessInfo(p, "continued");
    int pid = p->processID;
    kill(pid, SIGCONT);
    kill(pid, SIGUSR2);
}

void changeCurrentProcessRemainingTime()
{

    while (*shm_ptr == -1)
        ;
    currentProcess->remainingTime = *shm_ptr;
}

void stopProcess(PCB *current, int SIGNAL)
{
    *shm_ptr = -1;
    kill(current->processID, SIGNAL);
    while (*shm_ptr == -1)
        ;
    current->remainingTime = *shm_ptr;
    outProcessInfo(current, "stopped");
}

char *itoa(int num)
{
    int length = snprintf(NULL, 0, "%d", num);
    char *str = (char *)malloc(length + 1);
    snprintf(str, length + 1, "%d", num);
    return str;
}

int getTATime(PCB *p)
{
    return p->finishTime - p->arrivalTime;
}
float getWTATime(PCB *p)
{
    return getTATime(p) * 1.0 / p->totalRunTime;
}
void outFinishProcessInfo(PCB *p)
{
    p->remainingTime = 0;
    FILE *outFile = fopen("process_info.txt", "a");
    fprintf(outFile, "At time %d process %d finished arr : %d total : %d remain : %d wait : %d TA : %d WTA: %.2f \n", getClk(), p->id, p->arrivalTime, p->totalRunTime, p->remainingTime, getClk() - p->arrivalTime - p->totalRunTime, getTATime(p), getWTATime(p));
    printf("PID : %d - ID : %d\n", p->processID, p->id);
    fclose(outFile);
}

////////////////////////////////////////////////////////////////////////////////////

int getShmKey()
{
    int key = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = (int *)shmat(key, NULL, 0);
    return key;
}
#endif