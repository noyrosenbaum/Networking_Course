#include "callFunction.h"
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
#include <sys/time.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"
#define CHUNK 1024 // Read 1024 bytes at a time
#define BUFFER_SIZE 1024
#define FILE_SIZE 1060424

char buffer[BUFFER_SIZE];
int i = 0;

// char firstPart[(FILE_SIZE / 2)] = {0};
// char secondPart[(FILE_SIZE / 2)] = {0};
char exitMessage[5] = "Exit";

// Craete new socket
int createSocket(struct sockaddr_in *serverAddress)
{
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        printf("Could not create socket : %d\n", errno);
        return -1;
    }

    memset(serverAddress, 0, sizeof(*serverAddress));

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(SERVER_PORT);                                             // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress->sin_addr); // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0)
    {
        printf("inet_pton() failed\n");
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
        printf("send() failed with error code : %d\n", errno);
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

int userAnswers()
{
    char answer[4];
    char confirm[4];
    printf("Send the file again? Write 'yes' for sending again or 'Exit' for finish session.\n");
    scanf("%s", answer);
    printf("Your answer is: %s\n", answer);
    if (!strcmp(answer, "yes"))
    {
        printf("Running this proccess once again\n");
        return 1;
    }
    else
    {
        printf("Confirm exit with 'yes': \n");
        scanf("%s", confirm);
        if (strcmp(confirm, "yes"))
        {
            printf("Session is over\n");
            return 0;
        }
        else
        {
            // A chance for the user to change his mind
            userAnswers();
        }
    }
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
        printf("connect() failed with error code : %d\n", errno);
        // cleanup the socket;
        close(clientSocket);
        return -1;
    }

    printf("connected to server\n");

    //  Read 1MB file and sent it to Receiver
    FILE *pFile = fopen("test.txt", "r");
    // Move file's pointer to end of file
    long fileSize = fseek(pFile, 0, SEEK_END);
    long firstPartSize = fileSize / 2;
    long secondPartSize = fileSize - firstPartSize;
    // // read 1st part to a buffer
    // int freadFirstPart = fread(buffer, 1, sizeof(firstPartSize), pFile);
    // read 2nd part to a buffer
    int freadSecondPart = fread(buffer, 1, sizeof(secondPartSize), pFile);

    // while the answer is yes, run this while loop
    while (1)
    {
        while (++i < 5)
        {
            // send the first part of file
            // read 1st part to a buffer
            int freadFirstPart = fread(buffer, 1, sizeof(firstPartSize), pFile);
            sendToServer(clientSocket, buffer, freadFirstPart);

            // check authentication from server
            char authentication[] = "10000010111111";
            char bufferReply[BUFFER_SIZE] = {'\0'};
            int answer = recv(clientSocket, bufferReply, BUFFER_SIZE, 0);
            if (strcmp(bufferReply, authentication))
            {
                printf("Authentication Approved\n");
            }
            else
            {
                printf("Authentication is not approved\n");
            }

            // change to reno algorithm
            printf("Change to reno method\n");
            char CCReno[6] = "reno";
            if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CCReno, sizeof(CCReno)) != 0)
            {
                perror("setsockopt");
                return -1;
            }
            memset(buffer, 0, sizeof(*buffer));
            // send second part of file
            printf("Reach to before send sec part\n");

            // read 2nd part to a buffer
            int freadSecondPart = fread(buffer, 1, sizeof(secondPartSize), pFile);
            // WHY DOES IT SENDS ZERO?!?!?!?!?!?!?!?!
            sendToServer(clientSocket, buffer, freadSecondPart);
            printf("Sent the second part of the file\n");
        }

        // sending again? yes: while comtinues, no: gets out of while loop
        int userAnswer = userAnswers();
        if (userAnswer == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        // change algorithm if user's answer is yes and sends file once again
        printf("Change to cubic method\n");
        char CCCubic[6] = "cubic";
        if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CCCubic, sizeof(CCCubic)) != 0)
        {
            perror("setsockopt");
            return -1;
        }
    }
    // close file
    fclose(pFile);
    // exit message to reciever
    sendToServer(clientSocket, &exitMessage, BUFFER_SIZE);
    // close socket
    close(clientSocket);

    return 0;
}