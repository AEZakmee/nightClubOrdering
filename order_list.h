#ifndef ORDERLIST_
#define ORDERLIST_

#include <stdio.h>
#include <stdlib.h>

struct ListNode {
	struct MenuItem* tableOrder;
	int number;
	int table;
	int key;
	float total;
	struct ListNode* next;
};

struct ListNode* deleteFirst();
int isEmpty();
void freeList();
void addNewOrder(int table, int number, struct MenuItem *arr);
void addLast(int table, int number, struct MenuItem *arr);
void insertFirst(int table, int number, struct MenuItem *arr);
struct ListNode* delete(int key);
void completeOrder(int key, char* file, float *totalSales);
void printList();
int checkExistance(int order);

#endif