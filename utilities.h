#ifndef UTIL_
#define UTIL_

#include <stdio.h>
#include <stdlib.h>

struct MenuItem {
	int id;
	char name[20];
	float price;
	int number;
};

void err(char *str);
void sendItems(int socket, struct MenuItem *orderList, int count);
int checkInput(int min, int max, int input);
void getDailyReportFile(char *date);


#endif
