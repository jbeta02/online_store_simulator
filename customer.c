#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include "customerStruct.h"
#include "customer.h"

// genrate random num between 1-10
int getRandom() {
    srand(time(0)); // seed for rand num
    int randomNum = (rand() % 10) + 1; // 1-10
    
    return randomNum;
}

// convert char to useful int
float getPerceptionNum(struct Customer *customer) {
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
void changePerception(struct Customer *customer, char newPer) {
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
void incrementPerception(struct Customer *customer, int inc) {
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

// update daily cash taking into account brand perception
void recalcDailyCash(struct Customer *customer) {
    customer->dailyFreeCash = (customer->weeklyfreeCash) / 7 * getPerceptionNum(customer)/10;
}

// populate customer fields
struct Customer* populateCustomer(char *name_, int mIncome, char brandPer) {
    // use malloc to allocate space on the heap and edit fields using pointers
    struct Customer* customer = (struct Customer *) malloc(sizeof(struct Customer));
    strcpy(customer->name, name_);
    customer->monthlyIncome = mIncome;

    customer->weeklyfreeCash = (customer->monthlyIncome) / 4  * getRandom()/10; // out of the weekly cash take a percent to get the free weekly cash

    customer-> brandPerception = brandPer;

    changePerception(customer, brandPer);

    return customer;
}

// free heap
void destroyCustomers() {
    for (int i = 0; i < sizeof(customers)/sizeof(struct Customer*); i++) {
        if (customers[i] == NULL)
            break;
        
        free(customers[i]);
    }
}

// update relavent values from purchase
void purchase(struct Customer *customer) {
    if (customer->weeklyfreeCash > customer->dailyFreeCash && customer->monthlyIncome > customer->dailyFreeCash) { // check if there is enough money
        principle = principle + customer->dailyFreeCash;
        customer->monthlyIncome = customer->monthlyIncome - customer->dailyFreeCash;
        customer->weeklyfreeCash = customer->weeklyfreeCash - customer->dailyFreeCash;

        recalcDailyCash(customer);
    }
}
