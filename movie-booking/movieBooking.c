#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 50
#define MAX_SIZE 50
#define DISCOUNT 0.10f
#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_TICKETS 5

const int standardPrice = 150;
const int premiumPrice = 250;
const int reclinerPrice = 400;

enum TicketCategory {
    STANDARD = 1,
    PREMIUM = 2,
    RECLINER = 3
};

void readCustomerName(char *name);
int validateName(char* name);
int readCategory();
int validateCategory(int category);
int readNumberOfTickets();
int validateNumberOfTicket(int numberOfTickets);
void getCategory(enum TicketCategory category,int * ticketPrice,char* categoryName);
float applyDiscount(int numberOfTickets,float subtotal);
void displayBill(char * name,char * categoryName,int numberOfTickets ,float subtotal,float discount,float finalBill);


int main(){
    char name[MAX_LENGTH];
    int numberOfTickets;
    int category;
    int ticketPrice;
    char categoryName[MAX_SIZE];
    printf("Enter the customer Details \n");
    readCustomerName(name);
    if(!validateName(name)){
        printf("Customer name must be a single name without spaces.\n");
        return 0;
    }

    numberOfTickets = readNumberOfTickets();

    if(!validateNumberOfTicket(numberOfTickets)){
        printf("Invalid number of tickets. Enter %d to %d tickets.\n",MIN_TICKETS, MAX_TICKETS);
        return 0;
    }
    category = readCategory();

    if(!validateCategory(category)){
        printf("Invalid category. Please select 1, 2, or 3.\n");
        return 0;
    }

    getCategory((enum TicketCategory)category,&ticketPrice,categoryName);
    float subtotal  = ticketPrice * numberOfTickets;
    float discount = applyDiscount(numberOfTickets , subtotal);
    float finalBill = subtotal-discount;
    displayBill(name,categoryName,numberOfTickets,subtotal,discount,finalBill);

}

void readCustomerName(char *name) {
    printf("Enter customer name: ");

    if (fgets(name, MAX_LENGTH, stdin) == NULL) {
        name[0] = '\0';
        return;
    }

    name[strcspn(name, "\r\n")] = '\0';
}


int validateName(char* name){
    if (strlen(name) == 0) {
        return 0;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (isspace((unsigned char)name[i])) {
            return 0;
        }
    }

    return 1;
}


int readCategory() {
    int category;

    printf("Select ticket category:\n");
    printf("1. Standard (Rs. %d)\n", standardPrice);
    printf("2. Premium  (Rs. %d)\n", premiumPrice);
    printf("3. Recliner (Rs. %d)\n", reclinerPrice);
    printf("Enter category (1-3): ");

    if (scanf("%d", &category) != 1) {
        return -1;
    }

    return category;
}


int validateCategory(int category){
    return category >= STANDARD && category <= RECLINER;

}

int readNumberOfTickets() {
    int numberOfTickets;

    printf("Enter number of tickets (%d-%d): ",MIN_TICKETS, MAX_TICKETS);

    if (scanf("%d", &numberOfTickets) != 1) {
        return -1;
    }

    return numberOfTickets;
}


int validateNumberOfTicket(int numberOfTickets){
    return numberOfTickets >= MIN_TICKETS && numberOfTickets <= MAX_TICKETS;
}



void getCategory(enum TicketCategory category,int * ticketPrice,char* categoryName){
    switch (category)
    {
    case STANDARD:
        *ticketPrice = standardPrice;
        strcpy(categoryName,"Standard");
        break;
    case PREMIUM:
        *ticketPrice = premiumPrice;
        strcpy(categoryName,"Premium");
        break;
    case RECLINER:
        *ticketPrice = reclinerPrice;
        strcpy(categoryName,"Recliner");
        break;
    default:
        *ticketPrice = 0;
        break;
    }
}

float applyDiscount(int numberOfTickets,float subtotal){
    float discount;
    if(numberOfTickets>=DISCOUNT_TICKETS){
        discount = subtotal*DISCOUNT;
    }else{
        discount = 0;
    }
    return discount;
}

void displayBill(char * name,char * categoryName,int numberOfTickets ,float subtotal,float discount,float finalBill){
    printf("\n===== Booking Summary =====\n");
    printf("Customer: %s\n",name);
    printf("Ticket Category: %s\n",categoryName);
    printf("Tickets: %d\n",numberOfTickets);
    printf("Subtotal: %.2f\n",subtotal);
    printf("Discount: %.2f\n",discount);
    printf("Final Bill: %.2f\n",finalBill);

}