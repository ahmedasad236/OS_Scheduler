#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct buddyMemory
{
    int size;
    struct buddyMemory *next;
    bool allocated;
} buddyMemory;
buddyMemory *createBuddyMemory(int size)
{
    buddyMemory *root;
    root = (buddyMemory *)(malloc(sizeof(buddyMemory)));
    root->size = size;
    root->next = NULL;
    root->allocated = 0;
    return root;
}
void printBuddyMemory(buddyMemory *root)
{
    if (!root)
    {
        printf("\n");
        return;
    }
    printf("%d %d - ", root->size, (int)root->allocated);
    printBuddyMemory(root->next);
}
buddyMemory *compareMin(buddyMemory *m1, buddyMemory *m2)
{
    if (m1 == NULL)
        return m2;
    if (m2 == NULL)
        return m1;
    return m1->size <= m2->size ? m1 : m2;
}
int buddyMemoryLength(buddyMemory *root)
{
    if (root == NULL)
        return 0;
    return 1 + buddyMemoryLength(root->next);
}
buddyMemory *findMinValidBuddy(buddyMemory *root, int size)
{
    if (root == NULL)
        return NULL;
    if (root->size < size || root->allocated)
    {
        return findMinValidBuddy(root->next, size);
    }
    return compareMin(root, findMinValidBuddy(root->next, size));
}
buddyMemory *insertAfter(buddyMemory *element, int size)
{
    if (!element)
    {
        perror("can't print after a NULL");
        return NULL;
    }
    buddyMemory *newProcess = createBuddyMemory(size);
    newProcess->next = element->next;
    element->next = newProcess;
    return newProcess;
}
buddyMemory *splitNode(buddyMemory *node)
{
    node->size /= 2;
    insertAfter(node, node->size);
    return node;
}
buddyMemory *splitTillFit(buddyMemory *node, int size)
{
    while (node->size / 2 >= size)
        node = splitNode(node);
    return node;
}
buddyMemory *insertBuddyMemoryProcess(buddyMemory *root, int size)
{
    buddyMemory *min = findMinValidBuddy(root, size);
    if (!min)
        return NULL;
    min = splitTillFit(min, size);
    min->allocated = 1;
    return min;
}
buddyMemory *getPrevNodeInfo(buddyMemory *root, buddyMemory *node, int *prevSize)
{
    if (root == node)
        return NULL;
    buddyMemory *temp = root;
    if (temp == NULL)
    {
        printf("root is null\n");
        return NULL;
    }
    while (temp && temp->next != node)
    {
        *prevSize += temp->size;
        temp = temp->next;
    }
    *prevSize += temp->size;
    return temp;
}
int getPrevNodeSize(buddyMemory *root, buddyMemory *node)
{
    int prevSize = 0;
    buddyMemory *temp = getPrevNodeInfo(root, node, &prevSize);
    return prevSize;
}
int getSmallestUnit(int size)
{
    int it = 1024;
    while (it != size)
    {
        if (it < size)
            size -= it;
        it /= 2;
    }
    return size;
}
typedef enum MERGE_DIRECTIONS
{
    LEFT,
    RIGHT
} MERGE_DIRECTIONS;
MERGE_DIRECTIONS getMergeDirection(int prevSize, int nodeSize)
{
    int smallestUnit = getSmallestUnit(prevSize);
    if (smallestUnit == nodeSize)
        return LEFT;
    return RIGHT;
}
buddyMemory *mergingTwoNodes(buddyMemory *prev, buddyMemory *next)
{
    prev->next = next->next;
    prev->size *= 2;
    free(next);
    return prev;
}
buddyMemory *mergeBuddyMemory(buddyMemory *cur, buddyMemory *next)
{
    buddyMemory *result = NULL;
    if (!next->allocated && !cur->allocated && cur->size == next->size)
        result = mergingTwoNodes(cur, next);
    return result;
}

buddyMemory *mergeNodesWithDirection(MERGE_DIRECTIONS direction, buddyMemory *prev, buddyMemory *node)
{
    buddyMemory *mergedNode = NULL;
    if (direction == LEFT)
        mergedNode = mergeBuddyMemory(prev, node);
    else
        mergedNode = mergeBuddyMemory(node, node->next);
    return mergedNode;
}
void deallocateBuddyMemory(buddyMemory *root, buddyMemory *node)
{
    if (node == NULL || buddyMemoryLength(root) == 1)
        return;
    int prevSize = 0;
    buddyMemory *prev = getPrevNodeInfo(root, node, &prevSize);
    MERGE_DIRECTIONS direction = getMergeDirection(prevSize, node->size);
    node->allocated = 0;
    buddyMemory *mergedNode = mergeNodesWithDirection(direction, prev, node);
    deallocateBuddyMemory(root, mergedNode);
}
