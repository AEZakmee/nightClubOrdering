#ifndef UTIL_
#define UTIL_

#include <stdio.h>
#include <stdlib.h>
#define PRODUCT_NAME_LENGTH 20

struct MenuItem {
	int id;
	char name[PRODUCT_NAME_LENGTH];
	float price;
	int number;
};

void err(char *str);
void sendItems(int socket, struct MenuItem *orderList, int count);
int checkInput(int min, int max, int input);
void getDailyReportFile(char *date);


#endif
