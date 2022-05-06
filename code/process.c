#include "headers.h"

/* Modify this file as needed*/
int remaining_time;
bool stopped = true;
int *shm_ptr;
void SIGINT_handler(int sig)
{
    if (stopped)
        perror("SIGINT_handler");
    stopped = true;
    signal(SIGINT, SIGINT_handler);
}
void SIGCONT_handler(int sig)
{
    if (!stopped)
        perror("SIGCONT_handler");
    stopped = false;
    signal(SIGCONT, SIGCONT_handler);
}

int main(int argc, char *argv[])
{
    initClk();
    if (argc < 3)
        perror("Usage: ./process <remaining time> <shm_key>");
    remaining_time = atoi(argv[1]);
    int shm_key = atoi(argv[2]);
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
    destroyClk(false);
    return 0;
}