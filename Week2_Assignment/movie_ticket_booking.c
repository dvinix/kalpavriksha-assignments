#include <stdio.h>

#define CUSTOMER_NAME_CAPACITY 50

enum {
    MIN_TICKETS = 1,
    MAX_TICKETS = 20,
    DISCOUNT_MIN_TICKETS = 5
};

enum {
    CATEGORY_STANDARD = 1,
    CATEGORY_PREMIUM = 2,
    CATEGORY_RECLINER = 3
};

static const double STANDARD_TICKET_PRICE = 150.0;
static const double PREMIUM_TICKET_PRICE = 250.0;
static const double RECLINER_TICKET_PRICE = 400.0;
static const double BULK_DISCOUNT_RATE = 0.10;

static int is_valid_ticket_count(int ticketCount)
{
    return ticketCount >= MIN_TICKETS && ticketCount <= MAX_TICKETS;
}

static int resolve_category(int categoryChoice, const char **categoryName, double *ticketPrice)
{
    switch (categoryChoice) {
        case CATEGORY_STANDARD:
            *categoryName = "Standard";
            *ticketPrice = STANDARD_TICKET_PRICE;
            return 1;
        case CATEGORY_PREMIUM:
            *categoryName = "Premium";
            *ticketPrice = PREMIUM_TICKET_PRICE;
            return 1;
        case CATEGORY_RECLINER:
            *categoryName = "Recliner";
            *ticketPrice = RECLINER_TICKET_PRICE;
            return 1;
        default:
            return 0;
    }
}

static double calculate_discount(double subtotal, int ticketCount)
{
    if (ticketCount >= DISCOUNT_MIN_TICKETS) {
        return subtotal * BULK_DISCOUNT_RATE;
    }
    return 0.0;
}

int main(void)
{
    char customerName[CUSTOMER_NAME_CAPACITY];
    int ticketCount = 0;
    int categoryChoice = 0;
    const char *categoryName = NULL;
    double ticketPrice = 0.0;
    double subtotal = 0.0;
    double discountAmount = 0.0;
    double finalBill = 0.0;

    if (scanf("%49s %d %d", customerName, &ticketCount, &categoryChoice) != 3) {
        printf("Error: Invalid input.\n");
        return 1;
    }

    if (!is_valid_ticket_count(ticketCount)) {
        printf("Error: Number of tickets must be between %d and %d.\n", MIN_TICKETS, MAX_TICKETS);
        return 1;
    }

    if (!resolve_category(categoryChoice, &categoryName, &ticketPrice)) {
        printf("Error: Invalid ticket category. Choose 1, 2, or 3.\n");
        return 1;
    }

    subtotal = ticketPrice * ticketCount;
    discountAmount = calculate_discount(subtotal, ticketCount);
    finalBill = subtotal - discountAmount;

    printf("Customer: %s\n", customerName);
    printf("Ticket Category: %s\n", categoryName);
    printf("Tickets: %d\n", ticketCount);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discountAmount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}