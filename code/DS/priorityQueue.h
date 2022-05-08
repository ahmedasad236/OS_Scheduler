typedef struct PCB PCB;
typedef struct priQueue
{
    int size;
    PCB *head;
    PCB *tail;
    PCB *top;
    PCB *(*priorityComp)(void *, PCB *);
} priQueue;

// send priority function as parameter
priQueue *createPriQueue(PCB *(*F)(void *, PCB *))
{
    priQueue *newQueue = (priQueue *)malloc(sizeof(priQueue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->top = NULL;
    // assign the function to the queue
    newQueue->priorityComp = F;
    return newQueue;
}
PCB *compHPFProcesses(void *q, PCB *newProcess)
{

    PCB *tempPtr = ((priQueue *)q)->head;
    while (tempPtr && newProcess->priority >= tempPtr->priority)
        tempPtr = tempPtr->next;
    return tempPtr;
}
PCB *compSRTNProcesses(void *q, PCB *newProcess)
{
    PCB *tempPtr = ((priQueue *)q)->head;
    while (tempPtr && newProcess->remainingTime >= tempPtr->remainingTime)
        tempPtr = tempPtr->next;
    return tempPtr;
}
void priQueueInsert(priQueue *q, PCB *newProcess)
{
    q->size++;
    if (!q->head)
    {
        q->top = q->head = q->tail = newProcess;
        newProcess->next = newProcess->prev = NULL;
        return;
    }

    // call function as parameter

    PCB *tempPtr = q->priorityComp(q, newProcess);
    if (!tempPtr)
    {
        newProcess->prev = q->tail;
        newProcess->next = NULL;
        q->tail->next = newProcess;
        q->tail = newProcess;
    }

    else if (tempPtr == q->head)
    {
        newProcess->prev = NULL;
        newProcess->next = q->head;
        q->head->prev = newProcess;
        q->head = newProcess;
    }
    else
    {
        newProcess->next = tempPtr;
        newProcess->prev = tempPtr->prev;
        tempPtr->prev->next = newProcess;
        tempPtr->prev = newProcess;
    }
}

PCB *dequeuePriQueue(priQueue *q)
{
    if (q->head == NULL)
    {
        perror("No process in the queue");
        return NULL;
    }
    // Do not forget to delete the pointer in the main
    PCB *tempPtr = q->head;
    if (tempPtr == q->tail)
    {
        q->head = q->tail = NULL;
    }
    else
    {
        q->head = q->head->next;
        q->head->prev = NULL;
    }

    q->size--;
    return tempPtr;
}

bool isPriorityQueueEmpty(priQueue *q)
{
    return q->size == 0;
}
void printPriQueue(priQueue *q)
{
    PCB *tempPtr = q->head;
    while (tempPtr)
    {
        printf("%d ", tempPtr->id);
        tempPtr = tempPtr->next;
    }
    printf("\n");
}