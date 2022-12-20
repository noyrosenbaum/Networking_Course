#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"
#define CHUNK 1024 // Read 1024 bytes at a time
#define BUFFER_SIZE 1024
#define FILE_SIZE 1060424
int maxBuffer = 1024;

char firstPart[(FILE_SIZE / 2)] = {0};
char secondPart[(FILE_SIZE / 2)] = {0};

// Craete new socket
int createSocket(struct sockaddr_in *serverAddress)
{
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    memset(serverAddress, 0, sizeof(*serverAddress));

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(SERVER_PORT);                                             // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress->sin_addr); // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0)
    {
        printf("inet_pton() failed");
        return -1;
    }
    printf("Sender's socket successfully created\n");
    return client_sock;
}

// send to server
int sendToServer(int sSocket, void *buffer, int maxBuffer)
{
    int bytesSend = send(sSocket, buffer, maxBuffer, 0);
    if (bytesSend == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(sSocket);
        return -1;
    }
    else if (bytesSend == 0)
    {
        printf("peer has closed the TCP connection prior to send().\n");
    }
    else if (bytesSend < maxBuffer)
    {
        printf("sent only %d bytes from the required %d.\n", bytesSend, maxBuffer);
    }
    else
    {
        printf("message was successfully sent.\n");
    }
    return bytesSend;
}

int main()
{
    // Create Client socket
    int clientSocket;
    struct sockaddr_in serverAddress;
    clientSocket = createSocket(&serverAddress);

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        // cleanup the socket;
        close(clientSocket);
        return -1;
    }

    printf("connected to server\n");


    // file
    //  Read 1MB file and sent it to Receiver
    FILE *message;
    message = fopen("test.txt", "r");
    int freadFirstPart = fread(firstPart, 1, sizeof(firstPart), message);
    int freadSecondPart = fread(secondPart, 1, sizeof(secondPart), message);

    // send the first part of file
    
        return 0;
}
