#include "../algorithms/common.h"
#include "../algorithms/RoundRobin.h"
#include "../algorithms/SRTN.h"
#include "../algorithms/HPF.h"

int main(int argc, char *argv[])
{
    signal(SIGUSR1, SIGUSR1_handler);
    // outFile = fopen("analysis.txt", "a");
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
