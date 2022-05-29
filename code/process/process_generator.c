#include "../DS/linkedList.h"
#include <sys/ipc.h>
#include <sys/msg.h>
bool breaK = false;
typedef struct PCB PCB;
void clearResources(int);
struct processBuff
{
    int id;
    int arrivalTime;
    int remainingTime;
    short priority;
    int memorySize;
    long mtype;
}; // size without type = 4 * 3 + 2 = 14;
typedef struct ALGORITHM_TYPE
{
    long mtype;
    int algoType;
} ALGORITHM_TYPE;
void initializingCodes()
{
    system("rm -rf process_info.txt");
    system("rm -rf test.txt");
    system("../helping/kill_running_processes.sh");
}
int main(int argc, char *argv[])
{
    initializingCodes();
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    char buffer[100];
    int id, priority, remainingTime, arrivalTime, memorySize;
    FILE *ptr;
    ptr = fopen("processes.txt", "r");
    fgets(buffer, 100, ptr);

    while (fscanf(ptr, "%d %d %d %d %d", &id, &arrivalTime, &remainingTime, &priority, &memorySize) == 5)
    {
        printf("memory size : %d\n", memorySize);
        PCB *newProcess = createNewProcess(id, arrivalTime, remainingTime, priority, memorySize);
        insertLast(newProcess);
    }
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    ALGORITHM_TYPE schedulerAlgorithm;
    do
    {
        printf("Choose of the following scedular algorithms\n1)HPF\t2)SRTN\t3)RR\n");
        scanf("%d", &schedulerAlgorithm.algoType);
    } while (schedulerAlgorithm.algoType < 1 || schedulerAlgorithm.algoType > 3);
    // 3. Initiate and create the scheduler and clock processes.
    // run clock process to start the timer
    system("gcc clk.c -o clk.out && ./clk.out &");
    // run schedular to start working with ready processes
    // system("gcc scheduler.c -o scheduler.out && ./scheduler.out &");

    // 4. Use this function after creating the clock process to initialize clock
    initClk(); // now we have access to the shared memory contains time

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // done in the linkedlist file

    // 6. Send the information to the scheduler at the appropriate time.
    // create the queue to communicate with the scheduler
    schedulerAlgorithm.mtype = 120;
    int msgq_scheduler_id;
    do
    {
        msgq_scheduler_id = msgget(88, 0666 | IPC_CREAT);
    } while (msgq_scheduler_id == -1);
    printf("queue with id = %d\n", msgq_scheduler_id);
    printf("%d - %ld\n", schedulerAlgorithm.algoType, schedulerAlgorithm.mtype);
    msgsnd(msgq_scheduler_id, &schedulerAlgorithm, 4, !IPC_NOWAIT);
    struct processBuff processTemp;
    processTemp.mtype = 0;
    printf("size : %ld\n", sizeof(processTemp) - sizeof(long));
    printf("size : %ld\n", 4 * sizeof(int) + sizeof(short));

    while (processTableLength > 0)
    {
        if (head->arrivalTime <= getClk())
        {
            printf("clk : %d\n", getClk());
            processTemp.id = head->id;
            processTemp.arrivalTime = head->arrivalTime;
            processTemp.priority = head->priority;
            processTemp.remainingTime = head->remainingTime;
            processTemp.memorySize = head->memorySize;
            printf("%d - %ld - %d\n", processTemp.id, processTemp.mtype, processTemp.memorySize);
            msgsnd(msgq_scheduler_id, &processTemp, 24, !IPC_NOWAIT);
            deleteFirst();
        }
    }
    // testing and ok to be deleted
    printf("done sending : %d\n", getClk());
    // to not destroy the clock till ctrl+c also for temporary cleaning
    while (1)
    {
        if (breaK)
            break;
    }
    printLinkList();

    // 7. Clear clock resources
    destroyClk(true);
    // added by aref for temporary cleaning
    msgctl(msgq_scheduler_id, IPC_RMID, (struct msqid_ds *)0);
}

void clearResources(int signum)
{
    breaK = true;
    // TODO Clears all resources in case of interruption
}
