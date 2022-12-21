#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


//Calls for Functions

//Reciever

int createSocket(struct sockaddr_in*);

int recvfileChunks(int, void*, int);

int sendToClient(int, void*, int);

//Sender

int createSocket(struct sockaddr_in*);

int sendToServer(int, void*, int);

int userAnswers();





