#ifndef CLUBSERVER_
#define CLUBSERVER_

#include <stdio.h>
#include <stdlib.h>

void *clientHandlerThread(void *arg);
void serverThread();
void *socketThread(void *arg);
void getOrder(int socket, int table);
void printMenu();
void finishOrder();
void handler(int num);
int shutDownServer();
void showReports();
int checkAvaliablility(struct MenuItem *arr, int size, char *buf);

#endif