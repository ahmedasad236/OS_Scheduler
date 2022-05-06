#include "linkedList.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "queue.h"
#include <string.h>
struct processBuff
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    long mtype;
}; // size without type = 4 * 3 + 2 = 14;

void moveToNextProcess(queue *runningProcesses)
{
    if (runningProcesses->current == NULL || runningProcesses->current->next == NULL)
        return;
    // send signal to the current process to stop it
    // send signal to next process to work
    runningProcesses->current = runningProcesses->current->next;
}
char *itoa(int num)
{
    int length = snprintf(NULL, 0, "%d", num);
    char *str = malloc(length + 1);
    snprintf(*str, length + 1, "%d", num);
    return str;
}
void startNewProcess(queue *runningProcesses)
{
    // run the process and send remaining time to it
    PCB *current = runningProcesses->current;
    int key = shmget(current->id, sizeof(int), 0666 | IPC_CREAT);
    current->shm_ptr = shmat(key, NULL, 0);
    char *processRunMessage = malloc(sizeof(char) * 100);
    snprintf(processRunMessage, 100, "gcc process.c -o process_%d && ./process_%d %d %d", current->id, current->id, current->remainingTime, current->id);
    system(processRunMessage);
}
void insertProcessRR(queue *runningProcesses)
{
    startNewProcess(runningProcesses);
    queueInsertPointer(runningProcesses, head);
    deleteFirst();
}
void RoundRobin(queue *runningProcesses)
{
    int clk = getClk();
    const int timeSlot = 1;
    while (1)
    {
        if (head && head->arrivalTime >= getClk())
            startNewProcess(runningProcesses);
        if (getClk() >= clk + timeSlot)
        {
            moveToNextProcess(runningProcesses);
            clk = getClk();
        }
    }
}

int main(int argc, char *argv[])
{
    queue *runningProcesses = createQueue();

    printf("hi\n");
    initClk();
    int msgq_processGenerator_id;
    do
    {
        msgq_processGenerator_id = msgget(88, 0666 | IPC_CREAT);
    } while (msgq_processGenerator_id == -1);

    struct processBuff processTemp;
    processTemp.mtype = 8;
    printf("%d\n", msgq_processGenerator_id);
    int msgrcv_val;
    while (1)
    {
        // we check if the message queue contains anyvalue
        // while loop for handling more than one process at the same time
        while (msgrcv(msgq_processGenerator_id, &processTemp, 14, 0, IPC_NOWAIT) != -1)
        {
            // printf("id=%d\n",processTemp.id);
            insertLast(processTemp.id, processTemp.arrivalTime,
                       processTemp.remainingTime, processTemp.priority);
        }
        // code for scheduling the processes
    }
    // upon termination release the clock resources.
    destroyClk(true);
}
