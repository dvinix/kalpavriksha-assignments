#include <stdio.h>
#include <stdlib.h>

typedef struct StackNode {
    int val;
    int minVal;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* head;
} MinStack;

MinStack* minStackCreate() {
    MinStack* obj = (MinStack*)malloc(sizeof(MinStack));
    obj->head = NULL;
    return obj;
}

void minStackPush(MinStack* obj, int val) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->val = val;

    if (obj->head == NULL || val < obj->head->minVal) {
        newNode->minVal = val;
    } else {
        newNode->minVal = obj->head->minVal;
    }

    newNode->next = obj->head;
    obj->head = newNode;
}

void minStackPop(MinStack* obj) {
    if (obj->head == NULL) {
        return;
    }

    StackNode* temp = obj->head;
    obj->head = obj->head->next;
    free(temp);
}

int minStackTop(MinStack* obj) {
    return obj->head->val;
}

int minStackGetMin(MinStack* obj) {
    return obj->head->minVal;
}

void minStackFree(MinStack* obj) {
    while (obj->head != NULL) {
        minStackPop(obj);
    }
    free(obj);
}

int main() {
    MinStack* minStack = minStackCreate();

    minStackPush(minStack, -2);
    minStackPush(minStack, 0);
    minStackPush(minStack, -3);

    printf("Current Min: %d\n", minStackGetMin(minStack));

    minStackPop(minStack);

    printf("Top Element: %d\n", minStackTop(minStack));
    printf("Current Min: %d\n", minStackGetMin(minStack));

    minStackFree(minStack);

    return 0;
}