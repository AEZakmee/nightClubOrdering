#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include "utilities.h"

void err(char *str) {
	perror(str);
	exit(1);
}

void sendItems(int socket, struct MenuItem *orderList, int count) {
	int recieved = 0;
	while (1) {
		if (send(socket, &count, sizeof(int), 0) < 0) printf("Error seinding data");
		for (int i = 0; i < count; i++) {
			if (send(socket, &orderList[i], sizeof(struct MenuItem), 0) < 0) printf("Error sending data");
		}
		if (recv(socket, &recieved, sizeof(int), 0) < 0) printf("Error recieving data");
		if (recieved == 1) break;
	}
}

int checkInput(int min, int max, int input) {
	if (input > max || input < min) return -1;
	return 0;
}

void getDailyReportFile(char *date) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(date, 20, "Rep/%d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

