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

struct ListNode* reverseBetween(struct ListNode* head, int left, int right) {
    if (head == NULL || left == right) {
        return head;
    }

    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    struct ListNode* prev = &dummy;
    for (int i = 1; i < left; i++) {
        prev = prev->next;
    }

    struct ListNode* curr = prev->next;
    for (int i = 0; i < right - left; i++) {
        struct ListNode* nextNode = curr->next;
        curr->next = nextNode->next;
        nextNode->next = prev->next;
        prev->next = nextNode;
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

    int left = 2;
    int right = 4;

    printf("Original: ");
    printList(head);

    head = reverseBetween(head, left, right);

    printf("Reversed (%d to %d): ", left, right);
    printList(head);

    freeList(head);

    return 0;
}