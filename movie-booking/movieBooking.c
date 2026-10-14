#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 50
#define MAX_SIZE 50
#define DISCOUNT 0.10f

const int StandardPrice = 150;
const int PremiumPrice = 250;
const int ReclinerPrice = 400;

enum TicketCategory {
    STANDARD = 1,
    PREMIUM = 2,
    RECLINER = 3
};

int validateName(char* name);
int validateCategory(int *category);
int validateNumberOfTicket(int *numberOfTickets);
void getCategory(enum TicketCategory category,int * ticket_price,char* categoryName);
float applyDiscount(int numberOfTickets,float subtotal);
void displayBill(char * name,char * categoryName,int numberOfTickets ,float subtotal,float discount,float finalBill);


int main(){
    char name[MAX_LENGTH];
    int numberOfTickets;
    int category;
    int ticket_price;
    char categoryName[MAX_SIZE];
    printf("Enter the customer Details \n");
    
    if(!validateName(name)){
        printf("Customer name must be a single name without spaces.\n");
        return 0;
    }

    if(!validateNumberOfTicket(&numberOfTickets)){
        return 0;
    }
    if(!validateCategory(&category)){
        return 0;
    }

    getCategory((enum TicketCategory)category,&ticket_price,categoryName);
    float subtotal  = ticket_price * numberOfTickets;
    float discount = applyDiscount(numberOfTickets , subtotal);
    float finalBill = subtotal-discount;
    displayBill(name,categoryName,numberOfTickets,subtotal,discount,finalBill);

}

int validateName(char* name){
    if (fgets(name, MAX_LENGTH, stdin) == NULL) {
        return 0;
    }
    name[strcspn(name, "\r\n")] = '\0';
    if (strchr(name, ' ') != NULL || strlen(name) == 0) {
        return 0;
    }
    return 1;
}


int validateCategory(int *category){
    if(scanf("%d",category)!=1 || *category<STANDARD || *category>RECLINER){
        printf("Invalid Number of category Please Enter in Between (1-3)\n");
        return 0;
    }
    return 1;

}


int validateNumberOfTicket(int *numberOfTickets){
    if(scanf("%d",numberOfTickets)!=1 || *numberOfTickets<1 || *numberOfTickets>20){
        printf("Invalid Number of ticket Please Enter in Between (1-20)\n");
        return 0;
    }
    return 1;
}



void getCategory(enum TicketCategory category,int * ticket_price,char* categoryName){
    switch (category)
    {
    case STANDARD:
        *ticket_price = StandardPrice;
        strcpy(categoryName,"Standard");
        break;
    case PREMIUM:
        *ticket_price = PremiumPrice;
        strcpy(categoryName,"Premium");
        break;
    case RECLINER:
        *ticket_price = ReclinerPrice;
        strcpy(categoryName,"Recliner");
        break;
    default:
        *ticket_price = 0;
        break;
    }
}

float applyDiscount(int numberOfTickets,float subtotal){
    float discount;
    if(numberOfTickets>=5){
        discount = subtotal*DISCOUNT;
    }else{
        discount = 0;
    }
    return discount;
}

void displayBill(char * name,char * categoryName,int numberOfTickets ,float subtotal,float discount,float finalBill){
    printf("Customer: %s\n",name);
    printf("Ticket Category: %s\n",categoryName);
    printf("Tickets: %d\n",numberOfTickets);
    printf("Subtotal: %.2f\n",subtotal);
    printf("Discount: %.2f\n",discount);
    printf("Final Bill: %.2f\n",finalBill);

}