#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define RECEIVER_PORT 5060
#define RECEIVER_IP_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 35342
#define FILE_SIZE 1060424

char firstPart[(FILE_SIZE / 2)];
char secondPart[(FILE_SIZE / 2)];
char exitMessage[5] = "Exit";
char authentication[] = "10000010111111"; // bitwise XOR between both IDs
char typeofcc[6] = "cubic";
int byterec = 0;
char answer[4];
char confirm[4];
int byteSend = 0;

int main()
{

    //  Read 1MB file
    FILE *message;
    message = fopen("test.txt", "r");
    fread(firstPart, 1, sizeof(firstPart), message);   // read the first part of file
    fread(secondPart, 1, sizeof(secondPart), message); // read the second part
    fclose(message);

    // Create a sender socket
    int senderSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (senderSocket == -1)
    {
        printf("Could not create socket : %d\n", errno);
        return -1;
    }
    struct sockaddr_in receiverAddress;
    memset(&receiverAddress, 0, sizeof(receiverAddress));
    receiverAddress.sin_family = AF_INET;                                                          // version ipv4
    receiverAddress.sin_port = htons(RECEIVER_PORT);                                               // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)RECEIVER_IP_ADDRESS, &(receiverAddress.sin_addr)); // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0)
    {
        printf("inet_pton() failed\n");
        return -1;
    }
    printf("Socket successfully created\n");

    // Make a connection to the receiver with socket SendingSocket
    int connectResult = connect(senderSocket, (struct sockaddr *)&receiverAddress, sizeof(receiverAddress));
    if (connectResult == -1)
    {
        printf("Connect() failed with error code : %d\n", errno);
        // cleanup the socket;
        close(senderSocket);
        return -1;
    }

    printf("Connected to receiver\n");

    // while the answer is yes, run this while loop
    while (1)
    {
        byteSend = send(senderSocket, firstPart, (FILE_SIZE / 2), 0);
        if (byteSend == -1)
        {
            printf("Send() failed with error code : %d\n", errno);
            close(senderSocket);
            return -1;
        }

        else if (byteSend == 0)
        {
            printf("Peer has closed the TCP connection prior to send().\n");
            return -1;
        }
        // check later what to do if not working
        else
        {
            printf("First part was successfully sent %d bytes.\n", byteSend);
        }

        printf("Waiting for ACK\n");

        char authenticationRep[16] = {0};

        byterec = recv(senderSocket, authenticationRep, sizeof(authenticationRep), 0);
        if (!strcmp(authenticationRep, authentication))
        {
            printf("Authentication Approved\n");
        }
        if (byterec == 0)
        {
            printf("Peer has closed the TCP connection prior to send().\n");
            close(senderSocket);
            return -1;
        }
        else if (byterec == -1)
        {
            printf("recv() failed with error code : %d\n", errno);
            close(senderSocket);
            return -1;
        }

        // change to reno algorithm
        printf("Change to reno method\n");
        strcpy(typeofcc, "reno");
        if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
        {
            perror("Setsockopt error");
            return -1;
        }

        // sending second part of file
        printf("Sending the second part of the file\n");
        int bytesSend = send(senderSocket, secondPart, (FILE_SIZE / 2), 0);
        if (bytesSend == -1)
        {
            printf("Send() failed with error code : %d\n", errno);
            close(senderSocket);
            return -1;
        }
        else if (bytesSend == 0)
        {
            printf("Peer has closed the TCP connection prior to send().\n");
        }
        else
        {
            printf("Second part was successfully sent %d bytes\n", byteSend);
        }

        char dummyvar = '\0';
        recv(senderSocket, &dummyvar, sizeof(char), 0);

        // checking user preferences
        printf("Send the file again? Write 'yes' for sending again or 'Exit' for finish session.\n");
        scanf(" %s", answer);

        send(senderSocket, &dummyvar, sizeof(char), 0);
        recv(senderSocket, &dummyvar, sizeof(char), 0);
        printf("Your answer is: %s\n", answer);

        if (!strcmp(answer, "yes"))
        {
            printf("Running this process once again\n");
            // change algorithm if user's answer is yes and sends file once again
            byterec = 0;
            printf("Change to cubic method\n");
            strcpy(typeofcc, "cubic");
            if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
            {
                perror("setsockopt");
                return -1;
            }
        }

        else
        {
            printf("Session is over\n");
            break;
        }
    }

    // close socket
    close(senderSocket);

    return 0;
}

// YAEL CODE WITH MY FIXES

// #include <sys/types.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include <errno.h>
// #include <signal.h>

// #include <sys/types.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include <errno.h>
// #include <signal.h>

// #define RECEIVER_PORT 5060
// #define RECEIVER_IP_ADDRESS "127.0.0.1"
// #define BUFFER_SIZE 1024
// #define FILE_SIZE 1060424

// char firstPart[(FILE_SIZE / 2)];
// char secondPart[(FILE_SIZE / 2)];
// char exitMessage[5] = "Exit";
// char authentication[] = "10000010111111"; // bitwise XOR between both IDs
// char bufferReply[BUFFER_SIZE] = {0};
// char typeofcc[256] = "cubic";
// int byterec = 0;
// char answer[4];
// char confirm[4];
// int byteSend = 0;

// int main()
// {

//     //  Read 1MB file
//     FILE *message;
//     message = fopen("test.txt", "r");
//     fread(firstPart, 1, sizeof(firstPart), message);   // read the first part of file
//     fread(secondPart, 1, sizeof(secondPart), message); // read the second part
//     fclose(message);

//     // Create a sender socket
//     int senderSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (senderSocket == -1)
//     {
//         printf("Could not create socket : %d\n", errno);
//         return -1;
//     }
//     struct sockaddr_in receiverAddress;
//     memset(&receiverAddress, 0, sizeof(receiverAddress));
//     receiverAddress.sin_family = AF_INET;                                                          // version ipv4
//     receiverAddress.sin_port = htons(RECEIVER_PORT);                                               // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
//     int rval = inet_pton(AF_INET, (const char *)RECEIVER_IP_ADDRESS, &(receiverAddress.sin_addr)); // convert IPv4 and IPv6 addresses from text to binary form
//     // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
//     if (rval <= 0)
//     {
//         printf("inet_pton() failed\n");
//         return -1;
//     }
//     printf("Socket successfully created\n");

//     // Make a connection to the receiver with socket SendingSocket
//     int connectResult = connect(senderSocket, (struct sockaddr *)&receiverAddress, sizeof(receiverAddress));
//     if (connectResult == -1)
//     {
//         printf("connect() failed with error code : %d\n", errno);
//         // cleanup the socket;
//         close(senderSocket);
//         return -1;
//     }

//     printf("connected to receiver\n");

//     // while the answer is yes, run this while loop
//     while (1)
//     {
//         // sending the first part of file
//         while (1)
//         {
//             byteSend = send(senderSocket, firstPart, BUFFER_SIZE, 0);
//             if (byteSend == -1)
//             {
//                 printf("send() failed with error code : %d\n", errno);
//                 close(senderSocket);
//                 return -1;
//             }

//             else if (byteSend == 0)
//             {
//                 printf("peer has closed the TCP connection prior to send().\n");
//                 return -1;
//             }
//             // check later what to do if not working
//             else
//             {
//                 printf("first part was successfully sent.\n");
//             }
//             break;
//         }

//         printf("waiting for ACK\n");
//         // check authentication from server
//         memset(bufferReply, 0, sizeof(bufferReply));
//         while (1)
//         {
//             byterec = recv(senderSocket, bufferReply, BUFFER_SIZE, 0);
//             if (!strcmp(bufferReply, authentication))
//             {
//                 printf("Authentication Approved\n");
//                 break;
//             }
//             if (byterec == 0)
//             {
//                 printf("peer has closed the TCP connection prior to send().\n");
//                 close(senderSocket);
//                 return -1;
//             }
//             else if (byterec == -1)
//             {
//                 printf("recv() failed with error code : %d\n", errno);
//                 close(senderSocket);
//                 return -1;
//             }
//         }

//         // change to reno algorithm
//         printf("Change to reno method\n");
//         strcpy(typeofcc, "reno");
//         if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
//         {
//             perror("setsockopt error");
//             return -1;
//         }

//         // sending second part of file
//         while (1)
//         {

//             printf("Sending the second part of the file\n");
//             int bytesSend = send(senderSocket, secondPart, BUFFER_SIZE, 0);
//             if (bytesSend == -1)
//             {
//                 printf("send() failed with error code : %d\n", errno);
//                 close(senderSocket);
//             }
//             else if (bytesSend == 0)
//             {
//                 printf("peer has closed the TCP connection prior to send().\n");
//             }

//             else
//             {
//                 printf("second part was successfully sent.\n");
//             }
//             break;
//         }

//         // checking user preferences
//         printf("Send the file again? Write 'yes' for sending again or 'Exit' for finish session.\n");
//         scanf(" %s", answer);
//         printf("Your answer is: %s\n", answer);

//         if (!strcmp(answer, "yes"))
//         {
//             printf("Running this process once again\n");
//             send(senderSocket, answer, BUFFER_SIZE, 0);
//         }

//         else
//         {
//             printf("Session is over\n");
//             break;
//         }

//         // change algorithm if user's answer is yes and sends file once again
//         byterec = 0;
//         printf("Change to cubic method\n");
//         strcpy(typeofcc, "cubic");
//         if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
//         {
//             perror("setsockopt");
//             return -1;
//         }
//     }

//     // exit message to reciever
//     send(senderSocket, exitMessage, BUFFER_SIZE, 0);
//     // close socket
//     close(senderSocket);

//     return 0;
// }

// NOY VERSION

// #include "callFunction.h"
// #include <stdio.h>
// #include <arpa/inet.h>
// #include <errno.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <signal.h>
// #include <sys/time.h>

// #define SERVER_PORT 5060
// #define SERVER_IP_ADDRESS "127.0.0.1"
// #define CHUNK 1024 // Read 1024 bytes at a time
// #define BUFFER_SIZE 1024
// #define FILE_SIZE 1060424

// char buffer[BUFFER_SIZE];
// int i = 0;

// // char firstPart[(FILE_SIZE / 2)] = {0};
// // char secondPart[(FILE_SIZE / 2)] = {0};
// char exitMessage[5] = "Exit";

// // Craete new socket
// int createSocket(struct sockaddr_in *serverAddress)
// {
//     int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (client_sock == -1)
//     {
//         printf("Could not create socket : %d\n", errno);
//         return -1;
//     }

//     memset(serverAddress, 0, sizeof(*serverAddress));

//     serverAddress->sin_family = AF_INET;
//     serverAddress->sin_port = htons(SERVER_PORT);                                             // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
//     int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress->sin_addr); // convert IPv4 and IPv6 addresses from text to binary form
//     // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
//     if (rval <= 0)
//     {
//         printf("inet_pton() failed\n");
//         return -1;
//     }
//     printf("Sender's socket successfully created\n");
//     return client_sock;
// }

// // send to server
// int sendToServer(int sSocket, void *buffer, int maxBuffer)
// {
//     int bytesSend = send(sSocket, buffer, maxBuffer, 0);
//     if (bytesSend == -1)
//     {
//         printf("send() failed with error code : %d\n", errno);
//         close(sSocket);
//         return -1;
//     }
//     else if (bytesSend == 0)
//     {
//         printf("peer has closed the TCP connection prior to send().\n");
//     }
//     else if (bytesSend < maxBuffer)
//     {
//         printf("sent only %d bytes from the required %d.\n", bytesSend, maxBuffer);
//     }
//     else
//     {
//         printf("message was successfully sent.\n");
//     }
//     return bytesSend;
// }

// int userAnswers()
// {
//     char answer[4];
//     char confirm[4];
//     printf("Send the file again? Write 'yes' for sending again or 'Exit' for finish session.\n");
//     scanf("%s", answer);
//     printf("Your answer is: %s\n", answer);
//     if (!strcmp(answer, "yes"))
//     {
//         printf("Running this proccess once again\n");
//         return 1;
//     }
//     else
//     {
//         printf("Confirm exit with 'yes': \n");
//         scanf("%s", confirm);
//         if (strcmp(confirm, "yes"))
//         {
//             printf("Session is over\n");
//             return 0;
//         }
//         else
//         {
//             // A chance for the user to change his mind
//             userAnswers();
//         }
//     }
// }

// int main()
// {
//     // Create Client socket
//     int clientSocket;
//     struct sockaddr_in serverAddress;
//     clientSocket = createSocket(&serverAddress);

//     // Make a connection to the server with socket SendingSocket.
//     int connectResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
//     if (connectResult == -1)
//     {
//         printf("connect() failed with error code : %d\n", errno);
//         // cleanup the socket;
//         close(clientSocket);
//         return -1;
//     }

//     printf("connected to server\n");

//     //  Read 1MB file and sent it to Receiver
//     FILE *pFile = fopen("test.txt", "r");
//     // Move file's pointer to end of file
//     long fileSize = fseek(pFile, 0, SEEK_END);
//     long firstPartSize = fileSize / 2;
//     long secondPartSize = fileSize - firstPartSize;
//     // // read 1st part to a buffer
//     // int freadFirstPart = fread(buffer, 1, sizeof(firstPartSize), pFile);
//     // read 2nd part to a buffer
//     int freadSecondPart = fread(buffer, 1, sizeof(secondPartSize), pFile);

//     // while the answer is yes, run this while loop
//     while (1)
//     {
//         while (++i < 5)
//         {
//             // send the first part of file
//             // read 1st part to a buffer
//             int freadFirstPart = fread(buffer, 1, sizeof(firstPartSize), pFile);
//             sendToServer(clientSocket, buffer, freadFirstPart);

//             // check authentication from server
//             char authentication[] = "10000010111111";
//             char bufferReply[BUFFER_SIZE] = {'\0'};
//             int answer = recv(clientSocket, bufferReply, BUFFER_SIZE, 0);
//             if (strcmp(bufferReply, authentication))
//             {
//                 printf("Authentication Approved\n");
//             }
//             else
//             {
//                 printf("Authentication is not approved\n");
//             }

//             // change to reno algorithm
//             printf("Change to reno method\n");
//             char CCReno[6] = "reno";
//             if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CCReno, sizeof(CCReno)) != 0)
//             {
//                 perror("setsockopt");
//                 return -1;
//             }
//             memset(buffer, 0, sizeof(*buffer));
//             // send second part of file
//             printf("Reach to before send sec part\n");

//             // read 2nd part to a buffer
//             int freadSecondPart = fread(buffer, 1, sizeof(secondPartSize), pFile);
//             // WHY DOES IT SENDS ZERO?!?!?!?!?!?!?!?!
//             sendToServer(clientSocket, buffer, freadSecondPart);
//             printf("Sent the second part of the file\n");
//         }

//         // sending again? yes: while comtinues, no: gets out of while loop
//         int userAnswer = userAnswers();
//         if (userAnswer == 1)
//         {
//             return 1;
//         }
//         else
//         {
//             return 0;
//         }
//         // change algorithm if user's answer is yes and sends file once again
//         printf("Change to cubic method\n");
//         char CCCubic[6] = "cubic";
//         if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, CCCubic, sizeof(CCCubic)) != 0)
//         {
//             perror("setsockopt");
//             return -1;
//         }
//     }
//     // close file
//     fclose(pFile);
//     // exit message to reciever
//     sendToServer(clientSocket, &exitMessage, BUFFER_SIZE);
//     // close socket
//     close(clientSocket);

//     return 0;
// }