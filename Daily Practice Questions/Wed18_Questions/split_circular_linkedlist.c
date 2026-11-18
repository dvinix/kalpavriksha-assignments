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

void splitCircularList(struct ListNode* head, struct ListNode** head1Ref, struct ListNode** head2Ref) {
    if (head == NULL) {
        *head1Ref = NULL;
        *head2Ref = NULL;
        return;
    }

    if (head->next == head) {
        *head1Ref = head;
        *head2Ref = NULL;
        return;
    }

    struct ListNode* slow = head;
    struct ListNode* fast = head;

    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    if (fast->next->next == head) {
        fast = fast->next;
    }

    *head1Ref = head;
    *head2Ref = slow->next;

    fast->next = slow->next;
    slow->next = head;
}

void printCircularList(struct ListNode* head) {
    if (head == NULL) {
        printf("Empty\n");
        return;
    }

    struct ListNode* curr = head;
    do {
        printf("%d -> ", curr->val);
        curr = curr->next;
    } while (curr != head);
    printf("%d\n", head->val);
}

void freeCircularList(struct ListNode* head) {
    if (head == NULL) {
        return;
    }

    struct ListNode* curr = head->next;
    while (curr != head) {
        struct ListNode* temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(head);
}

int main() {
    struct ListNode* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);
    head->next->next->next->next->next = head;

    printf("Input:  ");
    printCircularList(head);

    struct ListNode* list1 = NULL;
    struct ListNode* list2 = NULL;

    splitCircularList(head, &list1, &list2);

    printf("List 1: ");
    printCircularList(list1);

    printf("List 2: ");
    printCircularList(list2);

    freeCircularList(list1);
    freeCircularList(list2);

    return 0;
}