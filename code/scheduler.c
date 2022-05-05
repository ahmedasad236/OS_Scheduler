#include "linkedList.h"
#include <sys/ipc.h>
#include <sys/msg.h>

struct processBuff
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    long mtype;
};// size without type = 4 * 3 + 2 = 14;
const int sizeOfProcessBuff = sizeof(struct processBuff) - sizeof(long);

int main(int argc, char * argv[])
{
    printf("hi\n");
    initClk();
    
    // // establish connection with process generator
    int msgq_processGenerator_id;
    do {
        msgq_processGenerator_id = msgget(88 , 0666|IPC_CREAT);
    } while (msgq_processGenerator_id == -1);

    // //// testing => ok to be deleted
    // // FILE*ptr;
    // // ptr = fopen("shedulerTest.txt","w");
    // // fprintf(ptr,"queue id = %d\n",msgq_processGenerator_id);
    // ////
    // //TODO implement the scheduler :)
    struct processBuff processTemp;
    processTemp.mtype = 8;
    // msgsnd(msgq_processGenerator_id,&processTemp,sizeOfProcessBuff,!IPC_NOWAIT);
    printf("%d\n",msgq_processGenerator_id);
    int msgrcv_val;
    while (1)
    {
        // we check if the message queue contains anyvalue
        // while loop for handling more than one process at the same time
        msgrcv_val = msgrcv(msgq_processGenerator_id,&processTemp,sizeOfProcessBuff,8,IPC_NOWAIT);
        while(msgrcv_val != -1)
        {
            // printf("id=%d\n",processTemp.id);
            insertLast(processTemp.id,processTemp.arrivalTime,
                processTemp.remainingTime,processTemp.priority);
        }
        // code for scheduling the processes
    }

    //upon termination release the clock resources.
    // destroyClk(true);
}
