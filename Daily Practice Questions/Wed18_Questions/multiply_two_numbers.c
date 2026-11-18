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

struct ListNode* multiplyTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    if (l1 == NULL || l2 == NULL) {
        return createNode(0);
    }

    if ((l1->val == 0 && l1->next == NULL) || (l2->val == 0 && l2->next == NULL)) {
        return createNode(0);
    }

    struct ListNode* resultHead = createNode(0);
    struct ListNode* pos1 = resultHead;

    for (struct ListNode* p1 = l1; p1 != NULL; p1 = p1->next) {
        struct ListNode* pos2 = pos1;
        int carry = 0;

        for (struct ListNode* p2 = l2; p2 != NULL; p2 = p2->next) {
            int prod = p1->val * p2->val + pos2->val + carry;
            pos2->val = prod % 10;
            carry = prod / 10;

            if (p2->next != NULL || carry > 0) {
                if (pos2->next == NULL) {
                    pos2->next = createNode(0);
                }
                pos2 = pos2->next;
            }
        }

        while (carry > 0) {
            int sum = pos2->val + carry;
            pos2->val = sum % 10;
            carry = sum / 10;

            if (carry > 0) {
                if (pos2->next == NULL) {
                    pos2->next = createNode(0);
                }
                pos2 = pos2->next;
            }
        }

        if (p1->next != NULL) {
            if (pos1->next == NULL) {
                pos1->next = createNode(0);
            }
            pos1 = pos1->next;
        }
    }

    return resultHead;
}

void printReverseList(struct ListNode* head) {
    while (head != NULL) {
        printf("%d", head->val);
        if (head->next != NULL) {
            printf(" -> ");
        }
        head = head->next;
    }
    printf("\n");
}

void printForwardNumber(struct ListNode* head) {
    if (head == NULL) {
        return;
    }
    printForwardNumber(head->next);
    printf("%d", head->val);
}

void freeList(struct ListNode* head) {
    while (head != NULL) {
        struct ListNode* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    struct ListNode* num1 = createNode(3);
    num1->next = createNode(4);
    num1->next->next = createNode(2);

    struct ListNode* num2 = createNode(4);
    num2->next = createNode(6);

    printf("Number 1 (reverse): ");
    printReverseList(num1);

    printf("Number 2 (reverse): ");
    printReverseList(num2);

    struct ListNode* product = multiplyTwoNumbers(num1, num2);

    printf("Product (reverse):  ");
    printReverseList(product);

    printf("Product (forward):  ");
    printForwardNumber(product);
    printf("\n");

    freeList(num1);
    freeList(num2);
    freeList(product);

    return 0;
}