#ifndef FILEM_
#define FILEM_

#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

int getItemsNumber();
void loadItemsFromFile(struct MenuItem *arr, int number);
void logToFile(char *log, char *date);
void saveOrderToFile(char *file, struct MenuItem *order, int number);
void saveTotal(char *file, float total);
int printAllReportFiles();
int printDailyReport(char *file);

#endif