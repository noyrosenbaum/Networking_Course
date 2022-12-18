/*
        TCP/IP client
*/
//first half - cubic
//second half - reno

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

void authenticator() 
{
    //206530172
    //209498211
    // int XOR = ();
}

void sendToReceiver(int socket)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char buf[CHUNK];
    // Read 1MB file and sent it to Receiver
    size_t file_size;
    FILE *message;
    message = fopen("test.txt", "r");

    while ((file_size = fread(buf, 1, sizeof(buf), message)) > 0)
    {
        int bytesSent = send(socket, buf, CHUNK, 0);
        if (bytesSent == -1)
        {
            printf("send() failed with error code : %d.\n", errno);
        }
        else if (bytesSent == 0)
        {
            printf("peer has closed the TCP connection prior to send().\n");
        }
        else if (bytesSent < file_size)
        {
            printf("sent only %ld bytes from the required %d.\n", file_size, bytesSent);
        }
        else
        {
            printf("%s", buf);
        }
    }
    fclose(message);
}

int main() {
    //Create socket
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1) {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);                                              // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);  // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0) {
        printf("inet_pton() failed");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(client_sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1) {
        printf("connect() failed with error code : %d", errno);
        // cleanup the socket;
        close(client_sock);
        return -1;
    }

    printf("connected to server\n");

    // Sends some data to server
    sendToReceiver(client_sock);
    close(client_sock);

    //change to Reno//
    
    //Create socket
    int client_sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock2 == -1) {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    struct sockaddr_in serverAddress2;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress2.sin_family = AF_INET;
    serverAddress2.sin_port = htons(SERVER_PORT);                                              // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);  // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0) {
        printf("inet_pton() failed");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(client_sock2, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1) {
        printf("connect() failed with error code : %d", errno);
        // cleanup the socket;
        close(client_sock2);
        return -1;
    }

    printf("connected to server\n");

     printf("Change to Reno method\n");
     char BUF[BUFFER_SIZE];
     strcpy(BUF, "reno");
     if(setsockopt(client_sock2, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
     {
        perror("setsockopt");
        return -1;
     }

    // Sends some data to server
    sendToReceiver(client_sock2);
    close(client_sock2);

    // char buffer[BUFFER_SIZE] = {'\0'};
    // char message[] = "Hello, from the Client\n";
    // int messageLen = strlen(message) + 1;
    
    // int bytesSent = send(client_sock, message, messageLen, 0);

    // if (bytesSent == -1) {
    //     printf("send() failed with error code : %d", errno);
    // } else if (bytesSent == 0) {
    //     printf("peer has closed the TCP connection prior to send().\n");
    // } else if (bytesSent < messageLen) {
    //     printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
    // } else {
    //     printf("message was successfully sent.\n");
    // }
    
    // Receive data from server
    char bufferReply[BUFFER_SIZE] = {'\0'};
    int bytesReceived = recv(client_sock, bufferReply, BUFFER_SIZE, 0);
    if (bytesReceived == -1) {
        printf("recv() failed with error code : %d", errno);
    } else if (bytesReceived == 0) {
        printf("peer has closed the TCP connection prior to recv().\n");
    } else {
        printf("received %d bytes from server: %s\n", bytesReceived, bufferReply);
    }
    
    close(client_sock);
    return 0;
}
