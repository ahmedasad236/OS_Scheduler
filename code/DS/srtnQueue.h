typedef struct PCB PCB;
typedef struct srtnQueue
{
    int size;
    PCB *head;
    PCB *tail;
    PCB *top;
} srtnQueue;

srtnQueue *createSrtnQueue()
{
    srtnQueue *newQueue = (srtnQueue *)malloc(sizeof(srtnQueue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->top = NULL;
    return newQueue;
}

void linkTwoPointers(PCB *p1, PCB *p2)
{
    p2->next = p1->next;
    p1->next->prev = p2;
    p1->next = p2;
    p2->prev = p1;
}

void srtnQueueInsert(srtnQueue *q, PCB *newProcess)
{
    q->size++;
    if (!q->head)
    {
        q->top = q->head = q->tail = newProcess;
        newProcess->next = newProcess->prev = NULL;
        return;
    }
    PCB *tempPtr = q->head;
    while (tempPtr->next && newProcess->remainingTime >= tempPtr->remainingTime)
        tempPtr = tempPtr->next;

    if (tempPtr == q->head)
    {
        newProcess->next = q->head;
        newProcess->prev = NULL;
        q->head->prev = newProcess;
        q->head = newProcess;
    }
    else if (tempPtr == q->tail)
    {
        if (newProcess->remainingTime >= q->tail->remainingTime)
        {
            q->tail->next = newProcess;
            newProcess->next = NULL;
            newProcess->prev = q->tail;
            q->tail = newProcess;
        }
        else
            linkTwoPointers(q->tail->prev, newProcess);
    }

    else
        linkTwoPointers(tempPtr->prev, newProcess);
}

PCB *dequeueSrtnQueue(srtnQueue *q)
{
    if (q->head == NULL)
    {
        perror("No process in the queue");
        return NULL;
    }
    // Do not forget to delete the pointer in the main
    PCB *tempPtr = q->head;
    q->head = q->head->next;
    q->head->prev = NULL;
    q->size--;
    return tempPtr;
}

bool isSrtnEmpty(srtnQueue *q)
{
    return q->size == 0;
}
