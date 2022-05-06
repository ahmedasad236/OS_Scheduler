#include "linkedList.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "queue.h"
struct processBuff
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    long mtype;
}; // size without type = 4 * 3 + 2 = 14;

void RoundRobin()
{
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
