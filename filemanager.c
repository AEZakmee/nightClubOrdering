#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "filemanager.h"
#include "utilities.h"
#define FILE_NAME "items.txt"
#define MENU_FILE "menu.txt"
#define LOG_FILE "log.txt"


int getItemsNumber() {

	struct stat st;
	stat(MENU_FILE, &st);
	int size = st.st_size;

	int count = size / sizeof(struct MenuItem);
	if (count < 1)
		err("No items");

	return count;
}


void loadItemsFromFile(struct MenuItem *arr, int number) {
	int fd = open(MENU_FILE, O_RDONLY, 0777);

	for (int i = 0; i < number; i++) {
		if (read(fd, &arr[i], sizeof(struct MenuItem)) < 0)
			err("Error reading file");
		arr[i].id = i;
	}
	close(fd);
}

void saveMenu(struct MenuItem *arr, int number) {
	int fd = open(MENU_FILE, O_WRONLY | O_CREAT, 0777);
	for (int i = 0; i < number; i++) {
		if (write(fd, &arr[i], sizeof(struct MenuItem)) < 0)
			err("Error writing to file");
	}
}

void logToFile(char *log, char *date) {
	int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0777);
	write(fd, date, strlen(date));
	write(fd, log, strlen(log));
	close(fd);
}

void saveOrderToFile(char *file, struct MenuItem *order, int number) {
	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0777);
	char data[50];
	for (int i = 0; i < number; i++) {
		memset(data, 0, 50);
		snprintf(data, 50, "%s-%d-%.2f\n", order[i].name, order[i].number, order[i].number * order[i].price);
		write(fd, data, strlen(data));
	}
	close(fd);
}

void saveTotal(char *file, float total) {
	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0777);
	char data[50];
	memset(data, 0, 50);
	snprintf(data, 50, "Total for the day: %.2f$\n", total);
	write(fd, data, strlen(data));
	close(fd);
}
int printAllReportFiles() {
	int n = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir("./Rep");
	printf("All avaliable reports:\n");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strlen(dir->d_name) > 10) {
				n++;
				printf("\t%s\n", dir->d_name);
			}
		}
		closedir(d);
	}
	return n;
}

static inline int get_ch(int fd)
{
	char c;
	if (read(fd, &c, 1) == 1)
		return (unsigned char)c;
	return EOF;
}
int printDailyReport(char *file) {
	int n = 1;
	char file_to_open[50];
	memset(file_to_open, 0, 50);
	snprintf(file_to_open, 50, "./Rep/%s.txt", file);
	int fd = open(file_to_open, O_RDONLY, 0777);
	if (fd == -1) {
		return 0;
	}
	printf("\n---Report data for %s---\n\n", file);
	int c;
	while ((c = get_ch(fd)) != EOF) {
		printf("%c", c);
	}
	return n;
}