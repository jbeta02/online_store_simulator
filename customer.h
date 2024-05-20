#ifndef CUSTOMER
#define CUSTOMER
#include "customerStruct.h"

// genrate random num between 1-10
int getRandom();

// convert char to useful int
float getPerceptionNum(struct Customer *customer);

// change percetpion customer has on store
void changePerception(struct Customer *customer, char newPer);

// change percetpion customer has on store
void incrementPerception(struct Customer *customer, int inc);

// update daily cash taking into account brand perception
void recalcDailyCash(struct Customer *customer);

// populate customer fields
struct Customer* populateCustomer(char *name_, int mIncome, char brandPer);

// free heap
void destroyCustomers();

// update relavent values from purchase
void purchase(struct Customer *customer);

#endif