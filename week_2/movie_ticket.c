#include <stdio.h>

const float Standard = 150.0f;
const float Premium = 250.0f;
const float Recliner = 400.0f;

struct Customer
{
    char name[50];
    int total_ticket;
    int category_choice;
};

float subTotal(int total_tickets, int category_choice)
{
    float amount;
    switch (category_choice)
    {
    case 1:
        amount = Standard;
        break;
    case 2:
        amount = Premium;
        break;
    case 3:
        amount = Recliner;
        break;
    default:
        amount = 0;
        break;
    }
    return amount * total_tickets;
}

float discount(int total_tickets, float subtotal)
{
    float discount = 0.0f;
    if (total_tickets >= 5)
    {
        discount = subtotal * 0.10f;
    }
    return discount;
}

int main()
{
    struct Customer customer;
    printf("Enter Customer's Name: ");
    scanf("%49s", customer.name);
    printf("Enter number of tickets: ");
    scanf("%d", &customer.total_ticket);
    if (customer.total_ticket < 1 || customer.total_ticket > 20)
    {
        printf("Invalid Total Tickets must be between 1 to 20.");
        return 1;
    }
    printf("Choose the category:\n1.Standard\n2.Premium\n3.Recliner");
    scanf("%d", &customer.category_choice);
    if (customer.category_choice < 1 || customer.category_choice > 3)
    {
        printf("Invalid Category Choice must be 1, 2, or 3.");
        return 1;
    }

    printf("Customer: %s\n", customer.name);
    if (customer.category_choice == 1)
    {
        printf("Ticket Category: Standard\n");
    }
    else if (customer.category_choice == 2)
    {
        printf("Ticket Category: Premium\n");
    }
    else
    {
        printf("Ticket Category: Recliner\n");
    }

    printf("Tickets: %d\n", customer.total_ticket);
    float subtotal = subTotal(customer.total_ticket, customer.category_choice);
    printf("Subtotal: %.2f\n", subtotal);
    float disc = discount(customer.total_ticket, subtotal);
    printf("Discount: %.2f\n", disc);
    printf("Final Bill: %.2f\n", subtotal - disc);
    return 0;
}