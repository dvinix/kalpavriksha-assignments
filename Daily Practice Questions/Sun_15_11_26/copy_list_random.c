#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node *next;
    struct Node *random;
};

struct Node* createNode(int val) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->val = val;
    newNode->next = NULL;
    newNode->random = NULL;
    return newNode;
}

struct Node* copyRandomList(struct Node* head) {
    if (head == NULL) {
        return NULL;
    }

    struct Node* curr = head;
    while (curr != NULL) {
        struct Node* copy = createNode(curr->val);
        copy->next = curr->next;
        curr->next = copy;
        curr = copy->next;
    }

    curr = head;
    while (curr != NULL) {
        if (curr->random != NULL) {
            curr->next->random = curr->random->next;
        }
        curr = curr->next->next;
    }

    struct Node dummy;
    dummy.val = 0;
    dummy.next = NULL;
    struct Node* copyTail = &dummy;

    curr = head;
    while (curr != NULL) {
        struct Node* copy = curr->next;
        curr->next = copy->next;

        copyTail->next = copy;
        copyTail = copy;

        curr = curr->next;
    }

    return dummy.next;
}

void printList(struct Node* head) {
    while (head != NULL) {
        printf("[%d, random: ", head->val);
        if (head->random != NULL) {
            printf("%d]", head->random->val);
        } else {
            printf("NULL]");
        }
        if (head->next != NULL) {
            printf(" -> ");
        }
        head = head->next;
    }
    printf("\n");
}

void freeList(struct Node* head) {
    while (head != NULL) {
        struct Node* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    struct Node* n1 = createNode(7);
    struct Node* n2 = createNode(13);
    struct Node* n3 = createNode(11);
    struct Node* n4 = createNode(10);
    struct Node* n5 = createNode(1);

    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = n5;

    n1->random = NULL;
    n2->random = n1;
    n3->random = n5;
    n4->random = n3;
    n5->random = n1;

    printf("Original: ");
    printList(n1);

    struct Node* cloned = copyRandomList(n1);

    printf("Cloned:   ");
    printList(cloned);

    freeList(n1);
    freeList(cloned);

    return 0;
}