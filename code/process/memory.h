// #include "../globals/headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct buddyMemory
{
    int size;
    struct buddyMemory *left;
    struct buddyMemory *right;
    bool allocated;
} buddyMemory;
buddyMemory *createBuddyTree(int size)
{
    struct buddyMemory *root;
    root = (buddyMemory *)(malloc(sizeof(buddyMemory)));
    root->size = size;
    root->left = root->right = NULL;
    root->allocated = 0;
    return root;
}
buddyMemory *compareMin(buddyMemory *m1, buddyMemory *m2)
{
    if (m1 == NULL)
        return m2;
    if (m2 == NULL)
        return m1;
    return m1->size <= m2->size ? m1 : m2;
}
buddyMemory *findMinValidBuddy(buddyMemory *root, int size)
{
    if (root == NULL || root->size < size)
        return NULL;
    buddyMemory *left = findMinValidBuddy(root->left, size);
    buddyMemory *right = findMinValidBuddy(root->right, size);
    if (root->left == NULL && root->right == NULL && !root->allocated)
        return root;
    return compareMin(left, right);
}

int buddyMemorySize(buddyMemory *root)
{
    if (root == NULL)
        return 0;
    return 1 + buddyMemorySize(root->left) + buddyMemorySize(root->right);
}
buddyMemory *insertProcess(buddyMemory *root, int size)
{
    buddyMemory *min = findMinValidBuddy(root, size);

    if (min == NULL)
        return NULL;
    while (min->size / 2 >= size)
    {
        min->left = createBuddyTree(min->size / 2);
        min->right = createBuddyTree(min->size / 2);
        min = min->left;
    }
    min->allocated = 1;
    return min;
}
void printTree(buddyMemory *root)
{
    if (!root)
        return;
    printf("%d %d ", root->size, (int)root->allocated);
    printTree(root->left);
    printTree(root->right);
}
int main()
{
    buddyMemory *root = createBuddyTree(1024);
    printf("SIZE : %d\n", buddyMemorySize(root));
    int sizes[] = {240, 20, 90, 10, 150, 9000, 30, 400};
    int length = sizeof(sizes) / sizeof(int);
    // length = 3;
    int i = 0;
    for (i = 0; i < length; i++)
        printf("%d ", sizes[i]);
    printf("\n");
    for (i = 0; i < length; i++)
    {
        printf("%d:\n", sizes[i]);
        buddyMemory *temp = insertProcess(root, sizes[i]);
        printf("location : %p\n", temp);
        printf("SIZE : %d\n", buddyMemorySize(root));
        printTree(root);
        printf("\n");
    }
}