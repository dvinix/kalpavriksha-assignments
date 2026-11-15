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

struct ListNode* getKthNode(struct ListNode* current, int k) {
    while (current != NULL && k > 0) {
        current = current->next;
        k--;
    }
    return current;
}

struct ListNode* reverseKGroup(struct ListNode* head, int k) {
    if (head == NULL || k <= 1) {
        return head;
    }

    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    struct ListNode* groupPrev = &dummy;

    while (1) {
        struct ListNode* kthNode = getKthNode(groupPrev, k);
        if (kthNode == NULL) {
            break;
        }

        struct ListNode* groupNext = kthNode->next;

        struct ListNode* prev = groupNext;
        struct ListNode* curr = groupPrev->next;

        while (curr != groupNext) {
            struct ListNode* nextTemp = curr->next;
            curr->next = prev;
            prev = curr;
            curr = nextTemp;
        }

        struct ListNode* oldGroupHead = groupPrev->next;
        groupPrev->next = kthNode;
        groupPrev = oldGroupHead;
    }

    return dummy.next;
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
    struct ListNode* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);

    int k = 2;

    printf("Original List: ");
    printList(head);

    head = reverseKGroup(head, k);

    printf("Reversed (k=%d): ", k);
    printList(head);

    freeList(head);

    return 0;
}
