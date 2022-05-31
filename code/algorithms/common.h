#ifndef __COMMON_H__
#define __COMMON_H__
#include "../globals/global_variables.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../DS/queue.h"
#include <math.h>
#include <string.h>
#include "../DS/priorityQueue.h"
#include "../globals/headers.h"
int cnt = 0;
PCB *currentProcess = NULL;
int *shm_ptr = NULL;
bool currentDeleted = false;
float totalWTA = 0;
int processesCount = 0;
float totalWaitingTime = 0;
float totalWTASquared = 0;
int totalProcessesTimes = 0;
int key;

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
    FILE *outFile = fopen("scheduler.log", "a");
    fprintf(outFile, "At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), p->id, state, p->arrivalTime, p->totalRunTime, p->remainingTime, getClk() - p->arrivalTime - p->totalRunTime + p->remainingTime);
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
    printf("id : %d\n", newProcess->processID);
    outProcessInfo(newProcess, "started");
}
void SIGUSR1_handler(int sig)
{
    currentDeleted = true;
    signal(SIGUSR1, SIGUSR1_handler);
}
void continueProcess(PCB *p)
{
    // if (p->state)
    //     return;
    if (!p->state)
        outProcessInfo(p, "continued");
    int pid = p->processID;
    kill(pid, SIGCONT);
    kill(pid, SIGUSR2);
}
void outAllocateMemory(buddyMemory *memory, PCB *process)
{
    FILE *file = fopen("memory.log", "a");
    int prevSize = getPrevNodeSize(memory, process->memoryNode);
    fprintf(file, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), process->memorySize, process->id, prevSize, prevSize + process->memoryNode->size - 1);
    fclose(file);
}
void outDeallocateMemory(buddyMemory *memory, PCB *process)
{
    FILE *file = fopen("memory.log", "a");
    int prevSize = getPrevNodeSize(memory, process->memoryNode);
    fprintf(file, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(), process->memorySize, process->id, prevSize, prevSize + process->memoryNode->size - 1);
    fclose(file);
}
void removeFromWaitingList(priQueue *readyQueue, PCB *process)
{
    if (process->prev)
        process->prev->next = process->next;
    if (process->next)
        process->next->prev = process->prev;
    if (process == readyQueue->head)
        readyQueue->head = process->next;
    if (process == readyQueue->tail)
        readyQueue->tail = process->prev;
    process->next = NULL;
    process->prev = NULL;
    free(process);
}
void checkInWaitingList(priQueue *processes, priQueue *readyQueue, buddyMemory *memory)
{
    PCB *tempProcess = readyQueue->head;
    while (tempProcess)
    {
        buddyMemory *nodeMemory = insertBuddyMemoryProcess(memory, tempProcess->memorySize);
        if (nodeMemory)
        {
            tempProcess->memoryNode = nodeMemory;
            priQueueInsert(processes, createNewProcessP(tempProcess));
            outAllocateMemory(memory, tempProcess);
            printf("process with size : %d inserted in running processes 2\n", tempProcess->memorySize);
            PCB *temp = tempProcess;
            tempProcess = tempProcess->next;
            removeFromWaitingList(readyQueue, temp);
        }
        else
        {
            tempProcess = tempProcess->next;
        }
    }
}
void changeCurrentProcessRemainingTime()
{

    while (*shm_ptr == -1)
        ;
    currentProcess->remainingTime = *shm_ptr;
}
int getTATime(PCB *p)
{
    return p->finishTime - p->arrivalTime;
}
float getWTATime(PCB *p)
{
    return getTATime(p) * 1.0 / p->totalRunTime;
}
int getWaitedTime(PCB *p)
{
    return getClk() - p->arrivalTime - p->totalRunTime;
}
float getAvgWTA()
{
    return totalWTA / processesCount;
}
float getAvgWaitedTime()
{
    return totalWaitingTime / processesCount;
}
float getSTDWTA()
{
    if (processesCount == 1)
        return 0;
    return sqrt((totalWTASquared - processesCount * getAvgWTA() * getAvgWTA()) / (processesCount - 1));
}

float getCPUUtilization()
{
    if (getClk() == 1)
        return 100.0;
    return totalProcessesTimes * 100.0 / (getClk() - 1);
}
void outProcessesSummary(PCB *p)
{
    float WTA = getWTATime(p);
    totalWTA += WTA;
    totalWTASquared += WTA * WTA;
    processesCount++;
    totalWaitingTime += getWaitedTime(p);
    totalProcessesTimes += p->totalRunTime;
    FILE *file = fopen("scheduler.pref", "w");
    fprintf(file, "CPU utilization = %.2f%c\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f\n", getCPUUtilization(), '%', getAvgWTA(), getAvgWaitedTime(), getSTDWTA());
    fclose(file);
}
void outFinishProcessInfo(PCB *p)
{
    if (p->state)
        return;
    p->finishTime = getClk();
    p->remainingTime = 0;
    outProcessesSummary(p);
    FILE *outFile = fopen("scheduler.log", "a");
    fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), p->id, p->arrivalTime, p->totalRunTime, p->remainingTime, getWaitedTime(p), getTATime(p), getWTATime(p));
    printf("PID : %d - ID : %d\n", p->processID, p->id);
    fclose(outFile);
}

void stopProcess(PCB *current, int SIGNAL)
{
    *shm_ptr = -1;
    kill(current->processID, SIGNAL);
    while (*shm_ptr == -1)
        ;
    current->remainingTime = *shm_ptr;
    if (current->remainingTime <= 0)
    {
        printf("process %d finished at time %d\n", current->id, getClk());
        outFinishProcessInfo(current);
        current->state = 1;
    }
    else
    {
        outProcessInfo(current, "stopped");
    }
}

char *itoa(int num)
{
    int length = snprintf(NULL, 0, "%d", num);
    char *str = (char *)malloc(length + 1);
    snprintf(str, length + 1, "%d", num);
    return str;
}

////////////////////////////////////////////////////////////////////////////////////

int getShmKey()
{
    int key = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = (int *)shmat(key, NULL, 0);
    return key;
}
#endif