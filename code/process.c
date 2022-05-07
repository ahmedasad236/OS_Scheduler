#include "headers.h"
#include "global_variables.h"
/* Modify this file as needed*/
int remaining_time;
int *shm_ptr;

// void SIGUSR1_handler(int sig)
// {
//     signal(SIGUSR1, SIGUSR1_handler);
//     raise(SIGSTOP);
// }
// void SIGUSR2_handler(int sig)
// {
//     raise(SIGCONT);
//     signal(SIGUSR2, SIGUSR2_handler);
// }
// void SIGCONT_handler(int sig)
// {
//     printf("hello world\n");
//     signal(SIGCONT, SIGCONT_handler);
// }
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
    // signal(SIGUSR1, SIGUSR1_handler);
    // signal(SIGUSR2, SIGUSR2_handler);
    // signal(SIGCONT, SIGCONT_handler);
    while (remaining_time > 0)
    {
        // printf("process_id : %d,remaining_time : %d\n", getpid(), remaining_time);
    }
    kill(scheduler_id, SIGUSR1);
    // destroyClk(false);
    return 0;
}
