// Delete Nodes That Have a Greater Node on the Right
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

struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode* prev = NULL;
    struct ListNode* curr = head;

    while (curr != NULL) {
        struct ListNode* nextTemp = curr->next;
        curr->next = prev;
        prev = curr;
        curr = nextTemp;
    }

    return prev;
}

struct ListNode* deleteNodesWithGreaterOnRight(struct ListNode* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    head = reverseList(head);

    struct ListNode* curr = head;
    struct ListNode* maxNode = head;

    while (curr != NULL && curr->next != NULL) {
        if (curr->next->val < maxNode->val) {
            struct ListNode* temp = curr->next;
            curr->next = temp->next;
            free(temp);
        } else {
            curr = curr->next;
            maxNode = curr;
        }
    }

    return reverseList(head);
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
    int values[] = {12, 15, 10, 11, 5, 6, 2, 3};
    int n = sizeof(values) / sizeof(values[0]);

    struct ListNode* head = createNode(values[0]);
    struct ListNode* tail = head;
    for (int i = 1; i < n; i++) {
        tail->next = createNode(values[i]);
        tail = tail->next;
    }

    printf("Input:  ");
    printList(head);

    head = deleteNodesWithGreaterOnRight(head);

    printf("Output: ");
    printList(head);

    freeList(head);

    return 0;
}