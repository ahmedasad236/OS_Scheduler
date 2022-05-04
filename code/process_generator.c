#include "linkedList.h"
void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    char buffer[100];
    int id,priority,remainingTime,arrivalTime;
    FILE*ptr;
    ptr = fopen("processes.txt","r");
    fgets(buffer,100,ptr);

    while (fscanf(ptr,"%d %d %d %d",&id,&arrivalTime,&remainingTime,&priority) == 4)
        insertLast(id,arrivalTime,remainingTime,priority);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    short schedulerAlgorithm;
    do{
        printf("Choose of the following scedular algorithms\n1)HPF\t2)SRTN\t3)RR\n");
        scanf("%hd",&schedulerAlgorithm);
    }while(schedulerAlgorithm < 1 || schedulerAlgorithm > 3);
    // 3. Initiate and create the scheduler and clock processes.
    // run clock process to start the timer
    system("gcc clk.c -o clk.out;./clk.out &");
    // run schedular to start working with ready processes
    system("gcc scheduler.c -o scheduler.out;./scheduler.out &");

    // 4. Use this function after creating the clock process to initialize clock
    initClk();// now we have access to the shared memory contains time
    // To get time use this
    // int x = getClk();
    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // done in the linkedlist file
    // 6. Send the information to the scheduler at the appropriate time.
    while (true)
    {
        if(head->arrivalTime == getClk())
        {
            // printf("%d %d %d\n",head->id,head->arrivalTime,head->remainingTime);
            break;
        }
    }

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
