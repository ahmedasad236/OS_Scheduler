#include "headers.h"
#include "global_variables.h"
/* Modify this file as needed*/
int remaining_time;
bool stopped = true;
int *shm_ptr;
int main(int argc, char *argv[])
{
    initClk();
    if (argc < 3)
        perror("Usage: ./process <remaining time> <scheduler_id> ");
    FILE *f = fopen("test_file.txt", "w");
    remaining_time = atoi(argv[1]);
    int scheduler_id = atoi(argv[2]);
    int shm_id = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = (int *)shmat(shm_id, NULL, 0);
    *shm_ptr = getpid();
    int clk = getClk();
    while (remaining_time > 0)
    {
        if (stopped)
            continue;
        printf("process_id : %d,remaining_time : %d\n", getpid(), remaining_time);
    }
    kill(scheduler_id, SIGUSR1);
    destroyClk(false);
    return 0;
}
