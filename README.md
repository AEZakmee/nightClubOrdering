# nightClubOrdering
Night Club Ordering system. TCP Multithreaded Server and Clients writen in C, UNIX.

1. Put all the files in one directory.
2. Compile the server using - gcc clubServer.c order_list.c utilities.c filemanager.c -o server -lpthread
3. Compile the client using - gcc client.c utilities.c -o client -lpthread
4. Start the server - ./server
5. Start as many clients as you want - ./client
