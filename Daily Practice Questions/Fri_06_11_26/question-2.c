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

struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode dummy;
    dummy.val = 0;
    dummy.next = NULL;
    struct ListNode* tail = &dummy;

    while (l1 != NULL && l2 != NULL) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }

    if (l1 != NULL) {
        tail->next = l1;
    } else {
        tail->next = l2;
    }

    return dummy.next;
}

struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    if (lists == NULL || listsSize == 0) {
        return NULL;
    }

    int interval = 1;
    while (interval < listsSize) {
        for (int i = 0; i + interval < listsSize; i += interval * 2) {
            lists[i] = mergeTwoLists(lists[i], lists[i + interval]);
        }
        interval *= 2;
    }

    return lists[0];
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
    int k = 3;
    struct ListNode** lists = (struct ListNode**)malloc(k * sizeof(struct ListNode*));

    lists[0] = createNode(1);
    lists[0]->next = createNode(4);
    lists[0]->next->next = createNode(5);

    lists[1] = createNode(1);
    lists[1]->next = createNode(3);
    lists[1]->next->next = createNode(4);

    lists[2] = createNode(2);
    lists[2]->next = createNode(6);

    for (int i = 0; i < k; i++) {
        printf("List %d: ", i + 1);
        printList(lists[i]);
    }

    struct ListNode* mergedHead = mergeKLists(lists, k);

    printf("Merged List: ");
    printList(mergedHead);

    freeList(mergedHead);
    free(lists);

    return 0;
}
