#include <stdio.h>
#include <stdlib.h>
#include "order_list.h"
#include "utilities.h"
#include "filemanager.h"

struct ListNode *head = NULL;
struct ListNode *current = NULL;
int totalorderNumber = 0;

struct ListNode* deleteFirst() {
	struct ListNode *tempLink = head;
	head = head->next;
	return tempLink;
}
int isEmpty() {
	return head == NULL;
}
void freeList() {
	while (!isEmpty()) {
		struct ListNode *temp = deleteFirst();
		printf("Order of table %d, was not completed, deleting data\n", temp->table);
		free(temp);
	}
}

void addNewOrder(int table, int number, struct MenuItem *arr) {
	insertFirst(table, number, arr);
}

void insertFirst(int table, int number, struct MenuItem *arr) {
	//create a link
	struct ListNode *link = (struct ListNode*) malloc(sizeof(struct ListNode));
	link->tableOrder = (struct MenuItem*) malloc(sizeof(struct MenuItem) * number);
	link->table = table;
	link->number = number;
	for (int i = 0; i < number ; i++) {
		link->tableOrder[i] = arr[i];
		link->total += arr[i].number * arr[i].price;
	}
	link-> key = totalorderNumber;
	totalorderNumber++;
	//point it to old first node
	link->next = head;
	//point first to new first node
	head = link;
}

void addLast(int table, int number, struct MenuItem *arr)
{
	//create a new node
	struct ListNode *newNode = (struct ListNode*) malloc(sizeof(struct ListNode));
	newNode->tableOrder = (struct MenuItem*) malloc(sizeof(struct MenuItem) * number);
	newNode->table = table;
	newNode->number = number;
	newNode->next = NULL;
	for (int i = 0; i < number ; i++) {
		newNode->tableOrder[i] = arr[i];
		newNode->total += arr[i].number * arr[i].price;
	}
	newNode-> key = totalorderNumber;
	totalorderNumber++;

	if (head == NULL)
		head = newNode;
	else
	{
		struct ListNode *lastNode = head;

		while (lastNode->next != NULL)
		{
			lastNode = lastNode->next;
		}
		lastNode->next = newNode;
	}

}

struct ListNode* delete(int key) {

	struct ListNode* current = head;
	struct ListNode* previous = NULL;
	if (head == NULL) {
		return NULL;
	}
	while (current->key != key) {
		if (current->next == NULL) {
			return NULL;
		} else {
			previous = current;
			current = current->next;
		}
	}

	if (current == head) {
		head = head->next;
	} else {
		previous->next = current->next;
	}
	return current;
}

void completeOrder(int key, char* file, float *totalSales) {
	struct ListNode *temp = delete(key);
	*totalSales += temp->total;
	saveOrderToFile(file, temp->tableOrder, temp->number);
	free(temp);
}

int checkExistance(int order) {
	struct ListNode *ptr = head;
	int ans = 0;
	while (ptr != NULL) {

		if (ptr-> key == order) {
			ans = 1;
			break;
		}
		ptr = ptr->next;
	}
	return ans;
}

void printList() {
	struct ListNode *ptr = head;
	printf("-------------Orders--------------\n");
	while (ptr != NULL) {
		printf("Order number: %d\n", ptr->key);
		printf("Table: %d\n", ptr->table);
		printf("Products:\n");
		for (int i = 0 ; i < ptr->number ; i++) {
			printf("\tProduct:%s, Quantity:%d\n", ptr-> tableOrder[i].name, ptr-> tableOrder[i].number);
		}
		printf("Total price: %.1f\n", ptr-> total);
		ptr = ptr->next;
		printf("-----------------\n\n");
	}
}