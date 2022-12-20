/*
        TCP/IP client
*/
// first half - cubic
// second half - reno

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


void sendExitMessage(int socket)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char message[] = "EXIT\n";
    int messageLen = strlen(message) + 1;

    int bytesSent = send(socket, message, messageLen, 0);

    if (bytesSent == -1)
    {
        printf("send() failed with error code : %d", errno);
    }
    else if (bytesSent == 0)
    {
        printf("peer has closed the TCP connection prior to send().\n");
    }
    else if (bytesSent < messageLen)
    {
        printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
    }
    else
    {
        printf("message was successfully sent.\n");
    }
}

void sendFileToReceiver(int socket)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char buf[CHUNK];
    // Read 1MB file and sent it to Receiver
    size_t file_size;
    FILE *message;
    message = fopen("test.txt", "r");
    int sum = 0;
    // long int size = ftell(message);
    while ((file_size = fread(buf, 1, sizeof(buf), message)) > 0 && sum < file_size / 2)
    {
        int bytesSent = send(socket, buf, CHUNK, 0);
        sum += CHUNK;
        bzero(buffer, BUFFER_SIZE);
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

int main()
{
    // Create socket
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);                                             // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr); // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0)
    {
        printf("inet_pton() failed");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(client_sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        // cleanup the socket;
        close(client_sock);
        return -1;
    }

    printf("connected to server\n");

    // //sends size of file
    // char buffer[BUFFER_SIZE] = {'\0'};
    // char message[] = "Hello, from the Client\n";
    // int messageLen = strlen(message) + 1;

    // int bytesSent = send(sock, message, messageLen, 0);

    // if (bytesSent == -1) {
    //     printf("send() failed with error code : %d", errno);
    // } else if (bytesSent == 0) {
    //     printf("peer has closed the TCP connection prior to send().\n");
    // } else if (bytesSent < messageLen) {
    //     printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
    // } else {
    //     printf("message was successfully sent.\n");
    // }

    // Sends first part to server - Cubic
    int i = 0;
    while (i++ < 5)
    {
        sendToReceiver(client_sock);
    }

    // cc function
    // int changeCCAlgorithm(char* algorithm)
    // {
    //     char algorithm_type[256];
    //     strcpy(algorithm_type, algorithm);
    //     printf("Change to %s method\n", algorithm);
    //     char BUF[BUFFER_SIZE];
    //     strcpy(BUF, "reno");
    //     if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
    //     {
    //         perror("setsockopt");
    //         return -1;
    //     }
    // }

    // change to Reno
    printf("Change to reno method\n");
    char BUF[BUFFER_SIZE];
    strcpy(BUF, "reno");
    if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    // Sends second part to server - Reno
    int j = 0;
    while (j++ < 5)
    {
        sendToReceiver(client_sock);
    }

    // User decision if to send file again or exit
    char decision[5];
    gets(decision); // remove and put scantf
    printf("Send the file again? Write 'yes' for sending again or 'exit' for finish session.%s", decision);
    int yes = strcmp(decision, "yes");
    int exit = strcmp(decision, "exit");
    do
    {
        if (yes == 0)
        {
            printf("Sending file again\n");
            // change to cubic
            printf("Change to cubic method\n");
            char BUF[BUFFER_SIZE];
            strcpy(BUF, "cubic");
            if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
            {
                perror("setsockopt");
                return -1;
            }
            // go back to the part where we send the first file
            int k = 0;
            while (k++ < 5)
            {
                sendToReceiver(client_sock);
            }
            // change to reno
            printf("Change to reno method\n");
            char BUF[BUFFER_SIZE];
            strcpy(BUF, "reno");
            if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
            {
                perror("setsockopt");
                return -1;
            }
            //send sec part
            int p = 0;
            while (p++ < 5)
            {
                sendToReceiver(client_sock);
            }
        }
        else if (exit == 0)
        {
            printf("Exit the session\n");
            sendExitMessage(client_sock);
            close(client_sock);
        }
        else
        {
            printf("You didn't write 'yes' or 'exit'\n", errno);
        }
    } while (yes == 0);

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
    // char bufferReply[BUFFER_SIZE] = {'\0'};
    // int bytesReceived = recv(client_sock, bufferReply, BUFFER_SIZE, 0);
    // if (bytesReceived == -1)
    // {
    //     printf("recv() failed with error code : %d", errno);
    // }
    // else if (bytesReceived == 0)
    // {
    //     printf("peer has closed the TCP connection prior to recv().\n");
    // }
    // else
    // {
    //     printf("received %d bytes from server: %s\n", bytesReceived, bufferReply);
    // }

    // close(client_sock);
    return 0;
}
