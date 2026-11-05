#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

ListNode* createNode(int val) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->val = val;
    node->next = NULL;
    return node;
}

ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy;
    dummy.next = head;
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }

    while (fast != NULL) {
        fast = fast->next;
        slow = slow->next;
    }

    ListNode* nodeToRemove = slow->next;
    slow->next = slow->next->next;
    free(nodeToRemove);

    return dummy.next;
}

void printList(ListNode* head) {
    ListNode* curr = head;
    while (curr != NULL) {
        printf("%d -> ", curr->val);
        curr = curr->next;
    }
    printf("NULL\n");
}

void freeList(ListNode* head) {
    ListNode* curr = head;
    while (curr != NULL) {
        ListNode* next = curr->next;
        free(curr);
        curr = next;
    }
}

int main() {
    ListNode* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);

    printf("Original list: ");
    printList(head);

    int n = 2;
    printf("Removing %dnd node from end...\n", n);
    head = removeNthFromEnd(head, n);

    printf("Resulting list: ");
    printList(head);

    freeList(head);
    return 0;
}
