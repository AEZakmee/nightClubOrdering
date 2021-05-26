#ifndef CLIENT_
#define CLIENT_

#include <stdio.h>
#include <stdlib.h>

void printAvaliableItems();
void getAvaliableItems(int socket);
int checkIfExists(struct MenuItem *arr, int id, int totalItems);
int getNumber(struct MenuItem item);
void updateOrder(struct MenuItem *arr, int id, int number, int totalItems);
void printCurrentOrder(struct MenuItem *arr, int totalItems, int final);
int getClientOrder(int socket, struct MenuItem *orderList);
void handler(int num);

#endif