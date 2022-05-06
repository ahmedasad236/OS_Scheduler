typedef struct PCB PCB;
typedef struct srtnQueue
{
    int size;
    PCB *head;
    PCB *tail;
    PCB *top;
}srtnQueue;

srtnQueue *createSrtnQueue()
{
    srtnQueue *newQueue = (srtnQueue *)malloc(sizeof(srtnQueue));
    newQueue->size = 0;
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->top = NULL;
    return newQueue;
}

void linkTwoPointers(PCB* p1, PCB* p2) {
    p2->next = p1->next;
    p1->next->prev = p2;
    p1->next = p2;
    p2->prev = p1;
}

void srtnQueueInsert(srtnQueue* q, PCB *newProcess)
{
    
    q->size++;
    
    if(!head){
        q->top = q->head = newProcess;
        newProcess->next = q->tail = NULL;
        return;
    }
    
    PCB* tempPtr = q->head;
    while(tempPtr->next && newProcess->remainingTime >= tempPtr->remainingTime) tempPtr = tempPtr->next;
    
    
    if(tempPtr == q->tail){
        if(newProcess->remainingTime >= q->tail->remainingTime){
            q->tail->next = newProcess;
            newProcess->prev = q->tail;
            q->tail = newProcess;
        }
        else
            linkTwoPointers(q->tail->prev, newProcess);
    }

    else 
        linkTwoPointers(tempPtr->prev, newProcess);
}


void srtnQueueDeleteFirst(srtnQueue*q)
{
    if (q->head == NULL)
    {
        perror("No process in the queue");
        return;
    }
    q->top = q->head;
    q->head = q->head->next;
    q->top = q->head;
    free(q->top);
    q->size--;
}

PCB* peek(){
    return head;
}

bool isSrtnEmpty(srtnQueue*q)
{
    return q->size == 0;
}
