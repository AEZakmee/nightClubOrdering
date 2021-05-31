#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include "utilities.h"
#include "order_list.h"
#include "clubServer.h"
#include "filemanager.h"
#define BUFLEN 512
#define CLIENTPORT 7799
#define FILE_NAME_LENGTH 20
#define clear() printf("\033[H\033[J")

pthread_mutex_t lock;
struct MenuItem *items;
int itemsNumber;
time_t t;
char *report_file;
float totalSales;

void handler(int num){
    printf("\nAre you sure you want to shutdown the server");
    printf("\nType EXIT to continue: ");
    char ans[4];
    scanf("%[^\n]",ans);
    getchar();
    if(strcmp(ans,"EXIT") == 0){
    	shutDownServer();
        exit(0);
    }
}

int shutDownServer(){
	saveTotal(report_file, totalSales);
	logToFile("Server shutdown\n ", ctime(&t));
   	freeList();
   	free(report_file);
    free(items);
}


int main(void){
	signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    time(&t);
    report_file = malloc(FILE_NAME_LENGTH);
    getDailyReportFile(report_file);
	itemsNumber = getItemsNumber();
	items = (struct MenuItem *)malloc(sizeof(struct MenuItem) * itemsNumber); 
	loadItemsFromFile(items, itemsNumber);
	logToFile("Server started\n ", ctime(&t));
	totalSales = 0;
	pthread_t clientHandler;
    printf("Starting up server handlers...  Proccess PID: %d\n", getpid());
    pthread_create(&clientHandler, NULL, clientHandlerThread, NULL);
    sleep(1);
    serverThread();
    return 0;
}





//
//
//	CLIENT ASSOCIATED FUNCTIONS BELOW
//
//
void *clientHandlerThread(void *arg)
{
	int serverSocket, newSocket;
  	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
  	//Create the socket. 
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  	// Configure settings of the server address struct
  	// Address family = Internet 
  	serverAddr.sin_family = AF_INET;
  	//Set port number, using htons function to use proper byte order 
  	serverAddr.sin_port = htons(CLIENTPORT);
  	//Set IP address to localhost 
  	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  	//Set all bits of the padding field to 0 
  	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  	//Asures that the binding adress is not in use
  	int option = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	//Bind the address struct to the socket 
  	if(bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))<0)
  		err("Binding error");
  	//Listen on the socket, with 40 max connection requests queued 
  	if(listen(serverSocket,50)==0)
    	logToFile("Client handler starts listening\n ", ctime(&t));
  	else
    	err("Error with client handler\n");
    	pthread_t tid[30];
    	int i = 0;
    while(1)
    {
        if(i>=30-1) i =0;
        addr_size = sizeof serverStorage;
        //Accept call creates a new socket for the incoming connection
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        if( pthread_create(&tid[i], NULL, socketThread, &newSocket) < 0 )
        	logToFile("Failed to create thread\n ", ctime(&t));
		i++;
    }
    logToFile("Client handler shutdown\n ", ctime(&t));
	return 0;
}

void *socketThread(void *arg)
{
  	int newSocket = *((int *)arg);
  	sendItems(newSocket, items, itemsNumber);
  	int table = 0;
  	if(recv(newSocket, &table, sizeof(int), 0) < 0) logToFile("Error recieving table data\n ", ctime(&t));;
  	logToFile("New client connected\n ", ctime(&t));
  	getOrder(newSocket, table);
  	logToFile("Recieved new order\n ", ctime(&t));
  	close(newSocket);
  	pthread_exit(NULL);
}

void getOrder(int socket, int table){
	int received = 1;
	struct MenuItem *order;
	int ordersize = 0;
	//todo get table data
	while(1){
	    if(recv(socket, &ordersize, sizeof(int),0) < 0){
	    	logToFile("Error recieving data\n ", ctime(&t));
	    	received = 0;
	    }
	    order = (struct MenuItem *)malloc(sizeof(struct MenuItem) * ordersize);
		for(int i=0;i<ordersize;i++){
			if(recv(socket, &order[i], sizeof(struct MenuItem), 0) < 0){
				logToFile("Error recieving data\n ", ctime(&t));
	    		received = 0;
			}
		}
		if(send(socket, &received, sizeof(int), 0) < 0){
	    	logToFile("Error sending data\n ", ctime(&t));
	    }
	    if(received = 1){
	    	char answerbuffer[BUFLEN];
	    	memset(answerbuffer, 0, BUFLEN);

	    	pthread_mutex_lock(&lock);
	    	int avaliable = checkAvaliablility(order, ordersize, answerbuffer);
	    	pthread_mutex_unlock(&lock);

	    	if(avaliable){
	    		memset(answerbuffer, 0, BUFLEN);
	    		strcpy(answerbuffer, "Order recieved seccessfuly.\nWe will work on it as soon as possible!\n");
	    	}
	    	if(send(socket, answerbuffer, BUFLEN, 0) < 0){
	    		logToFile("Error sending data\n ", ctime(&t));
			}

			pthread_mutex_lock(&lock);
	    	addNewOrder(table, ordersize, order);
	    	pthread_mutex_unlock(&lock);
			break;
	    }
	}
	free(order);
}

int checkAvaliablility(struct MenuItem *arr, int size, char *buf){
	strcpy(buf, "We ran out of some of the items you have ordered:\n");
	size_t len = strlen(buf);
	int allAvaliable = 1;
	for(int i =0; i< size; i++){
		for(int j =0; j< itemsNumber ;j++){
			if(arr[i].id == items[j].id){
				if(arr[i].number > items[j].number){
					len = strlen(buf);
					snprintf(buf + len, BUFLEN - len, "%s, You ordered %d, but we can offer: %d\n", arr[i].name, arr[i].number, items[j].number);
					arr[i].number = items[j].number;
					allAvaliable = 0;
				}
				items[j].number-=arr[i].number;
			}
		}
	}
	if(!allAvaliable){
		float total = 0;
		for(int i =0; i< size; i++){
			total+= arr[i].price * arr[i].number;
		}
		len = strlen(buf);
		snprintf(buf + len, BUFLEN - len, "Price was recalculated and now is: %.1f\nSorry for the inconvenience.\n", total);
	}
	return allAvaliable;
}







//
//
//	SERVER ASSOCIATED FUNCTIONS BELOW
//
//
void finishOrder(){
	clear();
	printList();
	getchar();
	printf("\nDo you want to take any order? (yes/no) ");
	char answer[3];
	memset(answer, 0, 3);
    scanf("%[^\n]",answer);
    printf("");
    if(strcmp(answer,"yes") == 0){
      	int order = -1;
        while(1){
        	printf("\nWhich order: ");
    		scanf("%d", &order);
    		if(checkExistance(order)==1){
			    completeOrder(order, report_file, &totalSales);
			    printf("Order taken, press enter to continue.");
			    getchar();
			    return;
    		}else{
    			printf("\nOrder does not exist, try again.");
    		}
    	}
    }
}
void showReports(){
	int n = printAllReportFiles();
	if(n==0){
		printf("There are no report files at the moment.");
    	getchar();
	}else{
		getchar();
		printf("Please enter the date you want to check:");
		char fileans[FILE_NAME_LENGTH];
		memset(fileans, 0, FILE_NAME_LENGTH);
    	scanf("%[^\n]",fileans);
    	if(!printDailyReport(fileans)){
    		printf("Unable to locate report file with date\"%s\"", fileans);
    	}
    	getchar();
	}
}
void printMenu(){
	clear();
	printf("---------Main Menu---------\n");
	printf("1) View orders\n");
	printf("2) View daily reports\n");

}
int choice;
void serverThread()
{
    while(1){
    	printMenu();
    	printf("\nChoice: ");
    	scanf("%d", &choice);
    	switch(choice){
    		case 1: 
    			if(!isEmpty()){
					finishOrder();
				}else{
					printf("There are no orders at the moment.");
    				getchar();
				}
    			break;
    		case 2:
    			showReports();
    			break;
    		default:
    			printf("Incorect choice, press enter to try again.");
    			getchar();
    			break;
    	}
    	getchar();
    }
}



