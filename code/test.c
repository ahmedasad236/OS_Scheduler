#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PCB.h"
#include "queue.h"

int main()
{
    queue *q = createQueue();
    insertNewProcess(q, createNewProcess(1, 1, 1, 1));
    q->current = q->head;
    insertNewProcess(q, createNewProcess(2, 2, 2, 2));
    insertNewProcess(q, createNewProcess(3, 3, 3, 3));
    printQueue(q);
    // q->current = queueFind(q, 2);
    // insertNewProcess(q, createNewProcess(4, 4, 4, 4));
    // insertNewProcess(q, createNewProcess(5, 5, 5, 5));
    // printQueue(q);
}