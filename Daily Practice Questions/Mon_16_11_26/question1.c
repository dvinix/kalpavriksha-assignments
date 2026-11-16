// Find the Middle Node Without Counting Nodes

#include <stdio.h>
#include <stdlib.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode* createNode(int val) {
    struct ListNode* newNode = (struct ListNode*)malloc(sizeof(struct ListNode));
    newNode->val = val;
    newNode->next = NULL;
    return newNode;
}

struct ListNode* findMiddleNode(struct ListNode* head) {
    struct ListNode* slow = head;
    struct ListNode* fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

void printList(struct ListNode* head) {
    while (head != NULL) {
        printf("%d", head->val);
        if (head->next != NULL) {
            printf(" -> ");
        }
        head = head->next;
    }
    printf("\n");
}

void freeList(struct ListNode* head) {
    while (head != NULL) {
        struct ListNode* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    struct ListNode* oddList = createNode(1);
    oddList->next = createNode(2);
    oddList->next->next = createNode(3);
    oddList->next->next->next = createNode(4);
    oddList->next->next->next->next = createNode(5);

    printf("Linked List: ");
    printList(oddList);
    struct ListNode* mid1 = findMiddleNode(oddList);
    if (mid1 != NULL) {
        printf("Middle Node: %d\n\n", mid1->val);
    }

    struct ListNode* evenList = createNode(1);
    evenList->next = createNode(2);
    evenList->next->next = createNode(3);
    evenList->next->next->next = createNode(4);

    printf("Linked List: ");
    printList(evenList);
    struct ListNode* mid2 = findMiddleNode(evenList);
    if (mid2 != NULL) {
        printf("Middle Node: %d\n", mid2->val);
    }

    freeList(oddList);
    freeList(evenList);

    return 0;
}