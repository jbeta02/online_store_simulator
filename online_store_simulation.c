#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include "game_summary.c"

int principle;
struct Customer* customers[30];
pthread_mutex_t mutex;

// genrate random num between 1-10
int getRandom() {
    srand(time(0)); // seed for rand num
    int randomNum = (rand() % 10) + 1; // 1-10
    
    return randomNum;
}

struct Customer {
    char name[20];
    int monthlyIncome; // available 30k: 2500 ,  100k: 8300
    int weeklyfreeCash; // mIncome / 4 * 0.x where x is random int (free cash is a percent of total cash per day)
    int dailyFreeCash; // dCash / 7 
    char brandPerception; // A, B, C, D, F
};

float getPerceptionNum(struct Customer* customer) {
    char brandPer = customer->brandPerception;
    int per = 0;
    switch (brandPer)
    {
    case 'A':
        per = 9;
        break;
    case 'B':
        per = 8;
        break;
    case 'C':
        per =7;
        break;
    case 'D':
        per = 6;
        break;
    case 'F':
        per = 5;
        break;
    }
    return per;
}

// change percetpion customer has on store
void changePerception(struct Customer* customer, char newPer) {
    int per = 0;
    switch (newPer)
    {
    case 'A':
        per = 9;
        break;
    case 'B':
        per = 8;
        break;
    case 'C':
        per = 7;
        break;
    case 'D':
        per = 6;
        break;
    case 'F':
        per = 5;
        break;
    
    }
    customer->brandPerception = newPer;
    customer->dailyFreeCash = (customer->weeklyfreeCash) / 7 * per/10;
}

// change percetpion customer has on store
void incrementPerception(struct Customer* customer, int inc) {
    int per = getPerceptionNum(customer) + inc;
    char brandP = 'A';
    switch (per)
    {
    case 9:
        brandP = 'A';
        break;
    case 8:
        brandP = 'B';
        break;
    case 7:
        brandP = 'C';
        break;
    case 6:
        brandP = 'D';
        break;
    case 5:
        brandP = 'F';
        break;
    }
    customer->brandPerception = brandP;
    customer->dailyFreeCash = (customer->weeklyfreeCash) / 7 * per/10;
}

void recalcDailyCash(struct Customer* customer) {
    customer->dailyFreeCash = (customer->weeklyfreeCash) / 7 * getPerceptionNum(customer)/10;
}

// populate customer fields
struct Customer* populateCustomer(char* name_, int mIncome, char brandPer) {
    // use malloc to allocate space on the heap and edit fields using pointers
    struct Customer* customer = (struct Customer *) malloc(sizeof(struct Customer));
    strcpy(customer->name, name_);
    customer->monthlyIncome = mIncome;

    customer->weeklyfreeCash = (customer->monthlyIncome) / 4  * getRandom()/10; // out of the weekly cash take a percent to get the free weekly cash

    customer-> brandPerception = brandPer;

    changePerception(customer, brandPer);

    return customer;
}

void destroyCustomers() {
    for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
        if (customers[i] == NULL)
            break;
        
        free(customers[i]);
    }
    free(customers);
}

void purchase(struct Customer* customer) {

    if (customer->weeklyfreeCash > customer->dailyFreeCash && customer->monthlyIncome > customer->dailyFreeCash) { // check if there is enough money
        principle = principle + customer->dailyFreeCash;
        customer->monthlyIncome = customer->monthlyIncome - customer->dailyFreeCash;
        customer->weeklyfreeCash = customer->weeklyfreeCash - customer->dailyFreeCash;

        recalcDailyCash(customer);
    }
}

void* cashflow(void *) {
    //int* principle = (int*) principleAmount;

    // clean up buffer
    fflush(stdout);

    //struct Customer* customers[] = {populateCustomer("John", 2500, 1300, 'A'), populateCustomer("Dav", 3000, 1000, 'B'), populateCustomer("Mike", 1000, 200, 'C')};
    int week = 1;
    for (int days = 0; days < 30; days++) {
        if (days % 7 == 0) { // check if week has passed
            if (week >= 4) 
                week = 3; // prevent division by 0
            for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
                if (customers[i] == NULL)
                    break;
                customers[i]->monthlyIncome = customers[i]->monthlyIncome - customers[i]->dailyFreeCash;
                customers[i]->weeklyfreeCash = customers[i]->weeklyfreeCash - customers[i]->dailyFreeCash;

                customers[i]->weeklyfreeCash = (customers[i]->monthlyIncome) / (4-week)  * getRandom()/10; // out of the weekly cash take a percent to get the free weekly cash
                changePerception(customers[i], customers[i]->brandPerception);
            }
            week++; // count number of weeks that have passed
        }

        for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
            if (customers[i] == NULL)
                break;
            purchase(customers[i]);
        }
        sleep(1); // one day is 1 second
    }
    
    return NULL;
}

void* gamePrompt() {
    int option = 0;
    int adCost = 300;
    int qualityCost = 100;
    while (option != 4) {
        printf(
        "\n1) Increase advertisement spending (increase number customers) (-$%d)\n"
        "2) Increase quality of store (increase brand percepton of all customers by 1 grade) (-$%d)\n"
        "3) see balance\n"
        "4) Close store\n", adCost, qualityCost
        );
        scanf("%d", &option);

        int len = sizeof(customers)/sizeof(struct Customer*);
        if (option == 1 && principle > adCost) { // && check left first, if false don't check right just set whole conditional to false
            pthread_mutex_lock(&mutex); // use mutex to prevent modification to priciple and customers while customer is being added
            principle -= adCost;
            for (int i = 0; i < len; i++) {
                if (customers[i] == NULL){
                    customers[i] = populateCustomer("name", getRandom() * 500, 'D');
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
        }
        if (option == 2 && principle > qualityCost) {
            principle -= qualityCost;
            for (int i = 0; i < len; i++) {
                if (customers[i] == NULL) {
                    break;
                }
                pthread_mutex_lock(&mutex); // use mutex to prevent modification to priciple and customers while brand per is being changed
                incrementPerception(customers[i], 1);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    printf("closing store...\n");
}

int main() { // first last principle

    pthread_mutex_init(&mutex, NULL);
    int pid = fork();

    customers[0] = populateCustomer("John", 2500, 'F'), 
    customers[1] = populateCustomer("Dave", 3000, 'F');
    customers[2] = populateCustomer("Mike", 1000, 'F');
    customers[3] = populateCustomer("Ash", 2300, 'F');
    customers[4] = populateCustomer("Leo", 1500, 'F');
    // create pipe to read from customers in parent after wait (for game summeary purposes)
    
    if (pid == 0) { // check if in child
        // create threads
        int status;
        pthread_t t1_id, t2_id;
        principle = 1000;
        pthread_create(&t1_id, NULL, cashflow, NULL);
        pthread_create(&t2_id, NULL, gamePrompt, NULL);
        pthread_join(t1_id, NULL);
        pthread_join(t2_id, NULL);
    }

    wait(NULL);

    // print game balance and summary
    printf("principle: %d", principle);
    display();
    pthread_mutex_destroy(&mutex);
    destroyCustomers();
    return 0;
}
