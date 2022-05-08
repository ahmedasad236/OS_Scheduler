#include "../globals/headers.h"
#include "../globals/global_variables.h"
/* Modify this file as needed*/
int remaining_time;
int *shm_ptr;

int startTime;
void SIGUSR2_handler(int sig)
{
    startTime = getClk();
    signal(SIGUSR2, SIGUSR2_handler);
}
void SIGUSR1_handler(int sig)
{
    remaining_time -= (getClk() - startTime) > 0 ? (getClk() - startTime) : 0;
    // if (remaining_time < 0)
    //     remaining_time = 0;
    // *shm_ptr = remaining_time;
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
    shm_ptr = (int *)shmat(shm_id, NULL, 0);
    signal(SIGUSR1, SIGUSR1_handler);
    signal(SIGUSR2, SIGUSR2_handler);
    *shm_ptr = getpid();
    int clk = getClk();
    startTime = getClk();
    printf("process_id : %d\n", getpid());
    while (remaining_time > 0)
    {
        // printf("remaining time : %d, clk : %d , stt : %d\n", remaining_time, getClk(), startTime);
        if (remaining_time <= (getClk() - startTime))
            break;
    }
    printf("process finished ID : %d\n", getpid());
    kill(scheduler_id, SIGUSR1);
    return 0;
}
