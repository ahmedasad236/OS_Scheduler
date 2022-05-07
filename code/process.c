#include "headers.h"
#include "global_variables.h"
/* Modify this file as needed*/
int remaining_time;
int *shm_ptr;

int startTime;
void SIGUSR1_handler(int sig)
{
    remaining_time -= (getClk() - startTime) > 0 ? (getClk() - startTime) : 0;
    signal(SIGUSR1, SIGUSR1_handler);
    raise(SIGSTOP);
}
int main(int argc, char *argv[])
{
    initClk();
    if (argc < 3)
        perror("Usage: ./process <remaining time> <scheduler_id> ");
    FILE *f = fopen("test_file.txt", "w");
    remaining_time = atoi(argv[1]);
    int scheduler_id = atoi(argv[2]);
    int shm_id = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    int *shm_ptr = (int *)shmat(shm_id, NULL, 0);
    signal(SIGUSR1 , SIGUSR1_handler);
    printf("id : %d\n", *shm_ptr);
    *shm_ptr = getpid();
    int clk = getClk();
    startTime = getClk();
    while (remaining_time > 0)
    {
        if (*shm_ptr == -1 * getpid())
        {
            startTime = getClk();
            *shm_ptr = -1;
            // printf("my process continued\n");
        }
        // printf("process_id : %d,remaining_time : %d\n", getpid(), remaining_time);
        // printf("process # %d is running and remaining time : %d\n", getpid(), remaining_time);
    }
    printf("process finished ID : %d\n", getpid());
    // shmctl(shm_id, IPC_RMID, (struct shmid_ds *)0);
    kill(scheduler_id, SIGUSR1);
    // destroyClk(false);
    return 0;
}
