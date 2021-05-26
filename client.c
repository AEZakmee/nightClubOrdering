#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <limits.h> 
#include "utilities.h"
#include "client.h"
#define BUFLEN 512
#define CLIENTPORT 7799
#define clear() printf("\033[H\033[J")

struct MenuItem *items;
int avaliableItems;
int table;

void handler(int num){
    free(items);
   	exit(0);
}

int main(int argc, char** argv){
	signal(SIGINT, handler);
    signal(SIGQUIT, handler);

    if(argc != 2)
    {
      printf("Usage : %s <Table-Number>\n",argv[0]);
      exit(0);
    }
    char *p;
	errno = 0;
	long conv = strtol(argv[1], &p, 10);

	if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < INT_MIN) {
	    printf("Incorrect ussage\n");
	    exit(0);
	} else {
	    table = conv;    
	    printf("Table %d\n", table);
	}

  	clear();
  	char message[BUFLEN];
  	char buffer[BUFLEN];
  	int clientSocket;
  	struct sockaddr_in serverAddr;
  	socklen_t addr_size;

  	// Create the socket. 
  	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  	//Configure settings of the server address
 	// Address family is Internet 
  	serverAddr.sin_family = AF_INET;

  	//Set port number, using htons function 
  	serverAddr.sin_port = htons(CLIENTPORT);

 	//Set IP address to localhost
  	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    if( connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) < 0){
    	printf("connect failed\n");
    }
    getAvaliableItems(clientSocket);
    if(send(clientSocket, &table, sizeof(int), 0) < 0){
	    err("Error sending table information\n");
	}
	struct MenuItem orderList[avaliableItems];
    int count = getClientOrder(clientSocket, orderList);
    sendItems(clientSocket, orderList, count);
    clear();
	printCurrentOrder(orderList, count, 1);
	char *ans[BUFLEN];
	if(recv(clientSocket, ans, BUFLEN, 0) < 0){
	    err("Error getting information\n");
	}
	printf("\n%s", ans);
    close(clientSocket);
  return 0;
}


int getClientOrder(int socket, struct MenuItem *orderList){
	int orderCount = 0;
	int number = 0;
	int changing = 0;
	while(1){
		printAvaliableItems();
		printf("\nEnter the id of the item you want to recieve: ");
		int itemId = -1;
		while(checkInput(0, avaliableItems-1, itemId) == -1){
			scanf("%d", &itemId);
			getchar();
			int check = checkIfExists(orderList, itemId, orderCount);
			if(check>0){
				printf("\n\nYou have added this item already. Total of %d items\n", check);
				printf("Do you want to change this? (yes/no) ");
				char ans[3];
        		scanf("%[^\n]",ans);
        		getchar();
        		if(strcmp(ans,"yes") == 0){
          			number = getNumber(items[itemId]);
          			updateOrder(orderList, itemId, number, orderCount);
          		}
          		changing = 1;
			}
			if(checkInput(0, avaliableItems-1, itemId) == -1){
				printf("Enter valid id: ");
			}
		}
		if(!changing){
			number = getNumber(items[itemId]);
			orderList[orderCount] = items[itemId];
			orderList[orderCount].number = number;
			orderCount++;
		}
		changing = 0;
		clear();
		printCurrentOrder(orderList, orderCount, 0);
		printf("\nDo you want to add something else? (yes/no) ");
		char ans[3];
        scanf("%[^\n]",ans);
        getchar();
        if(strcmp(ans,"no") == 0)
          break;
	}
	return orderCount;
}

void printAvaliableItems(){
	clear();
	printf("\t\tAvaliable items in the menu\n\n");
	printf("Id\n");
	for(int i=0;i<avaliableItems;i++){
		printf("%d  Product %s, price: %.1f$, in stock: %d\n", items[i].id, items[i].name, items[i].price, items[i].number);
	}
}

void getAvaliableItems(int socket){
	int received = 1;
	free(items);
	while(1){
	    if(recv(socket, &avaliableItems,sizeof(int),0) < 0){
	    	printf("Error recieving data\n");
	    	received = 0;
	    }
	    items = (struct MenuItem *)malloc(sizeof(struct MenuItem) * avaliableItems);
		for(int i=0;i<avaliableItems;i++){
			if(recv(socket, &items[i], sizeof(struct MenuItem), 0) < 0){
				printf("Error recieving data\n");
	    		received = 0;
			}
		}
		if(send(socket, &received, sizeof(int), 0) < 0){
	    	err("Error sending data\n");
	    }
	    if(received = 1){
	    	printAvaliableItems();
			break;
	    }
	}
}

int checkIfExists(struct MenuItem *arr, int id, int totalItems){
	for(int i=0;i<totalItems;i++){
		if(arr[i].id == id) return arr[i].number;
	}
	return 0;
}

int getNumber(struct MenuItem item){
	printf("How many %s do you want? ", item.name);
	int number = -1;
	while(checkInput(1, item.number, number) == -1){
		scanf("%d", &number);
		getchar();
		if(checkInput(1, item.number, number) == -1){
			printf("Enter valid number: ");
		}
	}
	return number;
}

void updateOrder(struct MenuItem *arr, int id, int number, int totalItems){
	for(int i=0;i<totalItems;i++){
		if(arr[i].id == id){
			arr[i].number = number;
		}
	}
}

void printCurrentOrder(struct MenuItem *arr, int totalItems, int final){
	if(final) printf("\nFinal order: \n");
	else printf("Current order: \n");
	float totalPrice = 0;
	for(int i=0;i<totalItems;i++){
		printf("%d. %s, %d quantity, Total price of %.1f$\n", i, &arr[i].name, arr[i].number, arr[i].price * arr[i].number);
		totalPrice += arr[i].price * arr[i].number;
	}
	printf("\t\t\tTotal: %.1f$\n\n", totalPrice);
}