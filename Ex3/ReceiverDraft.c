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

#define RECEIVER_PORT 5060 // The port that the receiver listens
#define BUFFER_SIZE 35342
#define FILE_SIZE 1060424
char buffer[BUFFER_SIZE];
int bytesReceived = 0;
int bytesReceived2 = 0;
char exitMessage[5] = {0};
char typeofcc[6] = "cubic";
double elapsedCubic = 0;
double elapsedReno = 0;
int i = 1;
double avaragepartone = 0;
double avarageparttwo = 0;
int userResponse = 1;
double timesofpartone[1000];
double timesofparttwo[1000];
long seconds;
long microsec;

int main()
{

    // Create listening socket
    int listeningSocket;
    struct sockaddr_in receiverAddress;

    listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)

    if (listeningSocket == -1)
    {
        printf("Could not create listening socket : %d\n", errno);
        return -1;
    }
    else
        (printf("A new listening socket has been created\n"));

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("Setsockopt() failed with error code : %d\n", errno);
        return 1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //

    memset(&receiverAddress, 0, sizeof(receiverAddress));

    receiverAddress.sin_family = AF_INET;
    receiverAddress.sin_addr.s_addr = INADDR_ANY;    // any IP at this port (Address to accept any incoming messages)
    receiverAddress.sin_port = htons(RECEIVER_PORT); // network order (makes byte order consistent)

    // Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningSocket, (struct sockaddr *)&receiverAddress, sizeof(receiverAddress));
    if (bindResult == -1)
    {
        printf("Bind failed with error code : %d\n", errno);
        // close the socket
        close(listeningSocket);
        exit(1);
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    // 500 is a Maximum size of queue connection requests
    // number of concurrent connections
    int listenResult = listen(listeningSocket, 4);
    if (listenResult == -1)
    {
        printf("Listen() failed with error code : %d\n", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }
    printf("Listening to sender\n");

    // Build receiver information
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in senderAddress;
    socklen_t senderAddressLen = sizeof(senderAddress);
    memset(&senderAddress, 0, sizeof(senderAddress));
    senderAddressLen = sizeof(senderAddress);

    // Accepts incoming connections
    int senderSocket = accept(listeningSocket, (struct sockaddr *)&senderAddress, &senderAddressLen);
    if (senderSocket == -1)
    {
        printf("Listen failed with error code : %d\n", errno);
        // close the sockets
        close(listeningSocket);
        return -1;
    }

    printf("A new connection accepted\n");

    struct timeval beginCubic, endCubic;

    while (userResponse)
    {
        gettimeofday(&beginCubic, 0);
        long tot = 0;
        while (tot != (FILE_SIZE / 2))
        {
            bytesReceived = recv(senderSocket, buffer, BUFFER_SIZE, 0);
            tot += bytesReceived;
            if (bytesReceived == -1)
            {
                printf("Recv failed with error code : %d\n", errno);
                close(senderSocket);
                return -1;
            }
            if (bytesReceived == 0)
            {
                printf("Connection socket closed\n");
                userResponse = 0;
                break;
            }
        }

        if (!userResponse)
            break;

        printf("The first file has been recevied\n");
        gettimeofday(&endCubic, 0);
        seconds = endCubic.tv_sec - beginCubic.tv_sec;
        microsec = endCubic.tv_usec - beginCubic.tv_usec;
        elapsedCubic = seconds + microsec * 1e-6;
        timesofpartone[i - 1] = elapsedCubic;

        // Send authentication massage to sender
        // authentication massage - XOR last 4 digits of our IDs
        char authentication[] = "10000010111111";
        send(senderSocket, &authentication, sizeof(authentication), 0);
        printf("Sending ACK to send\n");

        // change algorithm to reno
        printf("Change to reno method\n");
        strcpy(typeofcc, "reno");
        if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
        {
            perror("Setsockopt error");
            return -1;
        }

        // receiving second part
        struct timeval beginReno, endReno;
        gettimeofday(&beginReno, 0);
        tot = 0;
        while (tot != (FILE_SIZE / 2))
        {
            bytesReceived = recv(senderSocket, buffer, BUFFER_SIZE, 0);
            tot += bytesReceived;
            if (bytesReceived == -1)
            {
                printf("Recv failed with error code : %d\n", errno);
                close(senderSocket);
                return -1;
            }
            if (bytesReceived == 0)
            {
                printf("Connection socket closed\n");
                userResponse = 0;
                break;
            }
        }

        if (!userResponse)
            break;

        printf("The second part has been received\n");
        gettimeofday(&endReno, 0);

        seconds = endReno.tv_sec - beginReno.tv_sec;
        microsec = endReno.tv_usec - beginReno.tv_usec;
        elapsedReno = seconds + microsec * 1e-6;
        timesofparttwo[i - 1] = elapsedReno;

        char dummyvar = '\0';

        send(senderSocket, &dummyvar, sizeof(char), 0);

        printf("Receiver waiting for sender decision\n");

        recv(senderSocket, &dummyvar, sizeof(char), 0);
        send(senderSocket, &dummyvar, sizeof(char), 0);
        i++;
    }

    close(senderSocket);
    close(listeningSocket);

    int j = 0;

    while (j + 1 < i)
    {
        printf("The first part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofpartone[j]);
        avaragepartone += timesofpartone[j];
        printf("The second part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofparttwo[j]);
        avarageparttwo += timesofparttwo[j];
        j++;
    }
    avaragepartone = avaragepartone / --i;
    avarageparttwo = avarageparttwo / i;
    if (i == 1)
        printf("There was only one iteration. no avarage time to print");
    else
    {
        printf("The avarge time of receiving part one is: %f\n", avaragepartone);
        printf("The avarge time of receiving part two is: %f\n", avarageparttwo);
    }

    printf("The total avarage time for receiving file is: %f\n", (avaragepartone + avarageparttwo) / 2);
    printf("Bye bye:)\n");

    return 0;
}

// YAEL CODE WITH MY FIXES

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

// #define RECEIVER_PORT 5060 // The port that the receiver listens
// #define BUFFER_SIZE 1024
// #define FILE_SIZE 1060424
// char buffer[BUFFER_SIZE];
// int bytesReceived = 0;
// int bytesReceived2 = 0;
// char exitMessage[5] = {0};
// char typeofcc[256] = "cubic";
// double elapsedCubic;
// double elapsedReno;
// int i = 1;
// double avaragepartone = 0;
// double avarageparttwo = 0;
// int userResponse = 1;
// double timesofpartone[10000];
// double timesofparttwo[10000];
// int sum = 0;

// int main()
// {

//     // Create listening socket
//     int listeningSocket;
//     struct sockaddr_in receiverAddress;

//     listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)

//     if (listeningSocket == -1)
//     {
//         printf("Could not create listening socket : %d\n", errno);
//         return -1;
//     }
//     else
//         (printf("a new listening socket has been created\n"));

//     // Reuse the address if the server socket on was closed
//     // and remains for 45 seconds in TIME-WAIT state till the final removal.
//     int enableReuse = 1;
//     int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
//     if (ret < 0)
//     {
//         printf("setsockopt() failed with error code : %d\n", errno);
//         return 1;
//     }

//     // "sockaddr_in" is the "derived" from sockaddr structure
//     // used for IPv4 communication. For IPv6, use sockaddr_in6
//     //

//     memset(&receiverAddress, 0, sizeof(receiverAddress));

//     receiverAddress.sin_family = AF_INET;
//     receiverAddress.sin_addr.s_addr = INADDR_ANY;    // any IP at this port (Address to accept any incoming messages)
//     receiverAddress.sin_port = htons(RECEIVER_PORT); // network order (makes byte order consistent)

//     // Bind the socket to the port with any IP at this port
//     int bindResult = bind(listeningSocket, (struct sockaddr *)&receiverAddress, sizeof(receiverAddress));
//     if (bindResult == -1)
//     {
//         printf("Bind failed with error code : %d\n", errno);
//         // close the socket
//         close(listeningSocket);
//         exit(1);
//     }

//     printf("Bind() success\n");

//     // Make the socket listening; actually mother of all client sockets.
//     // 500 is a Maximum size of queue connection requests
//     // number of concurrent connections
//     int listenResult = listen(listeningSocket, 4);
//     if (listenResult == -1)
//     {
//         printf("listen() failed with error code : %d\n", errno);
//         // close the socket
//         close(listeningSocket);
//         return -1;
//     }
//     printf("Listening to sender\n");

//     // Build receiver information
//     printf("Waiting for incoming TCP-connections...\n");
//     struct sockaddr_in senderAddress;
//     socklen_t senderAddressLen = sizeof(senderAddress);
//     memset(&senderAddress, 0, sizeof(senderAddress));
//     senderAddressLen = sizeof(senderAddress);

//     // Accepts incoming connections
//     int senderSocket = accept(listeningSocket, (struct sockaddr *)&senderAddress, &senderAddressLen);
//     if (senderSocket == -1)
//     {
//         printf("listen failed with error code : %d\n", errno);
//         // close the sockets
//         close(listeningSocket);
//         return -1;
//     }

//     printf("A new connection accepted\n");

//     struct timeval beginCubic, endCubic;

//     while (userResponse)
//     {
//         bzero(buffer, BUFFER_SIZE);
//         gettimeofday(&beginCubic, NULL);
//         userResponse = 0;
//         while (1)
//         {
//             bytesReceived = recv(senderSocket, buffer, BUFFER_SIZE, 0);
//             sum += bytesReceived;
//             printf("Received chunks: %d\n", bytesReceived);
//             if (sum >= (FILE_SIZE / 2))
//             {
//                 bzero(buffer, BUFFER_SIZE);
//                 gettimeofday(&endCubic, NULL);
//                 long seconds = endCubic.tv_sec - beginCubic.tv_sec;
//                 long microsec = endCubic.tv_usec - beginCubic.tv_usec;
//                 elapsedCubic = seconds + microsec * 1e-6;
//                 printf("Time for first part: %f seconds:\n", elapsedCubic);
//                 timesofpartone[i - 1] = elapsedCubic;
//             }
//             else if (bytesReceived == -1)
//             {
//                 printf("recv failed with error code : %d\n", errno);
//                 close(senderSocket);
//                 return -1;
//             }
//             else if (bytesReceived == 0)
//             {
//                 printf("connection socket closed\n");
//                 close(senderSocket);
//                 return -1;
//             }
//             else
//             {
//                 printf("First part of file is not fully sent\n");
//             }
//             break;
//         }
//         printf("Received chunks: %d\n", bytesReceived2);

//         // Send authentication massage to sender
//         // authentication massage - XOR last 4 digits of our IDs
//         char authentication[] = "10000010111111";
//         send(senderSocket, &authentication, sizeof(authentication), 0);

//         // change algorithm to reno
//         printf("Change to reno method\n");
//         strcpy(typeofcc, "reno");
//         if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, typeofcc, sizeof(typeofcc)) != 0)
//         {
//             perror("setsockopt error");
//             return -1;
//         }
//         bzero(buffer, BUFFER_SIZE);
//         sum = 0;
//         // receiving second part
//         struct timeval beginReno, endReno;
//         gettimeofday(&beginReno, NULL);
//         while (1)
//         {
//             bytesReceived2 = recv(senderSocket, buffer, BUFFER_SIZE, 0);
//             sum += bytesReceived2;
//             printf("Received chunks: %d\n", bytesReceived2);
//             if (sum >= (FILE_SIZE / 2))
//             {
//                 bzero(buffer, BUFFER_SIZE);
//                 gettimeofday(&endReno, NULL);
//                 long secondsReno = endReno.tv_sec - beginReno.tv_sec;
//                 long microsecReno = endReno.tv_usec - beginReno.tv_usec;
//                 elapsedReno = secondsReno + microsecReno * 1e-6;
//                 printf("Time for second part: %f seconds:\n", elapsedReno);
//                 timesofparttwo[i - 1] = elapsedReno;
//             }
//             if (bytesReceived2 == -1)
//             {
//                 printf("recv failed with error code : %d\n", errno);
//                 close(senderSocket);
//                 return -1;
//             }
//             if (bytesReceived2 == 0)
//             {
//                 printf("connection socket closed\n");
//                 close(senderSocket);
//                 return -1;
//             }

//             printf("the second part has been recevied\n");
//             break;
//         }
//         printf("Received chunks: %d\n", bytesReceived2);

//         int decs;
//         printf("receiver waiting for sender decision\n");
//         while ((decs = recv(senderSocket, buffer, BUFFER_SIZE, 0)) > 0)
//         {
//             if (strcmp(buffer, exitMessage) == 0)
//             {
//                 close(listeningSocket);
//                 close(senderSocket);
//                 userResponse = 0;
//                 break;
//             }

//             else
//             {
//                 userResponse = 1;
//                 printf("\n");
//                 printf("starting again\n");
//                 i++;
//                 bytesReceived2 = 0;
//                 bytesReceived = 0;
//                 break;
//             }
//         }
//         if (decs == -1)
//         {
//             printf("recv() error\n");
//             close(senderSocket);
//             return -1;
//         }
//         if (decs == 0)
//         {
//             printf("peer closed socket\n");
//             close(senderSocket);
//         }
//     }

//     int j = 0;
//     while (j < i)
//     {
//         printf("the first part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofpartone[j]);
//         avaragepartone += timesofpartone[j];
//         printf("the second part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofparttwo[j]);
//         avarageparttwo += timesofparttwo[j];
//         j++;
//     }
//     avaragepartone = avaragepartone / i;
//     avarageparttwo = avarageparttwo / i;
//     printf("the avarge time of receiving part one is: %f\n", avaragepartone);
//     printf("the avarge time of receiving part two is: %f\n", avarageparttwo);
//     printf("bye bye");

//     return 0;
// }

// NOY VERSION BELOW

// // Linux and other UNIXes
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

// #define SERVER_PORT 5060 // The port that the server listens
// #define BUFFER_SIZE 1024
// // #define CHUNK 1024 // Read 1024 bytes at a time
// #define FILE_SIZE 1060424
// char buffer[BUFFER_SIZE];

// int createSocket(struct sockaddr_in *serverAddress)
// {
//     // Open the listening (server) socket
//     int listeningSocket = -1;
//     listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
//     if (listeningSocket == -1)
//     {
//         printf("Could not create listening socket : %d\n", errno);
//         return 1;
//     }

//     // Reuse the address if the server socket on was closed
//     // and remains for 45 seconds in TIME-WAIT state till the final removal.
//     int enableReuse = 1;
//     int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
//     if (ret < 0)
//     {
//         printf("setsockopt() failed with error code : %d\n", errno);
//         return 1;
//     }

//     // "sockaddr_in" is the "derived" from sockaddr structure
//     // used for IPv4 communication. For IPv6, use sockaddr_in6
//     //
//     // struct sockaddr_in serverAddress;
//     memset(serverAddress, 0, sizeof(*serverAddress));

//     serverAddress->sin_family = AF_INET;
//     serverAddress->sin_addr.s_addr = INADDR_ANY;  // any IP at this port (Address to accept any incoming messages)
//     serverAddress->sin_port = htons(SERVER_PORT); // network order (makes byte order consistent)

//     // Bind the socket to the port with any IP at this port
//     int bindResult = bind(listeningSocket, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
//     if (bindResult == -1)
//     {
//         printf("Bind failed with error code : %d\n", errno);
//         // close the socket
//         close(listeningSocket);
//         exit(1);
//     }

//     printf("Bind() success\n");

//     // Make the socket listening; actually mother of all client sockets.
//     // 500 is a Maximum size of queue connection requests
//     // number of concurrent connections
//     int listenResult = listen(listeningSocket, 1);
//     if (listenResult == -1)
//     {
//         printf("listen() failed with error code : %d\n", errno);
//         // close the socket
//         close(listeningSocket);
//         return -1;
//     }
//     printf("Sender's socket successfully created\n");
//     return listeningSocket;
// }

// // Recieve chunks of file - when inserting parameters to function - we write the pointer of the wanted value
// int recvfileChunks(int cSocket, void *buffer, int maxBuffer)
// {
//     int bytesReceived = recv(cSocket, buffer, maxBuffer, 0);
//     if (bytesReceived == -1)
//     {
//         printf("recv failed with error code : %d\n", errno);
//         close(cSocket);
//         return -1;
//     }

//     printf("Received chunks: %d\n", bytesReceived);
//     return bytesReceived;
// }

// // send to client
// int sendToClient(int cSocket, void *buffer, int maxBuffer)
// {
//     int bytesSend = send(cSocket, buffer, maxBuffer, 0);
//     if (bytesSend == -1)
//     {
//         printf("send() failed with error code : %d\n", errno);
//         close(cSocket);
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

// int main()
// {
//     // Create Client socket
//     int serverSocket;
//     struct sockaddr_in serverAddress;
//     serverSocket = createSocket(&serverAddress);
//     printf("Listen to client\n");

//     // Build Client information
//     printf("Waiting for incoming TCP-connections...\n");
//     struct sockaddr_in clientAddress;
//     socklen_t clientAddressLen = sizeof(clientAddress);
//     memset(&clientAddress, 0, sizeof(clientAddress));
//     clientAddressLen = sizeof(clientAddress);

//     // Accepts incoming connections
//     int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
//     if (clientSocket == -1)
//     {
//         printf("listen failed with error code : %d\n", errno);
//         // close the sockets
//         close(serverSocket);
//         return -1;
//     }
//     printf("A new client connection accepted\n");

//     // sum the bytes so it will not pass half of file size
//     int sum = 0;
//     int i = 0;
//     struct timeval beginCubic, endCubic;
//     struct timeval beginReno, endReno;
//     double elapsedCubic;
//     double elapsedReno;

//     // measure time
//     while (1)
//     {
//         while (++i < 5)
//         {
//             // measure with cubic - default in linux
//             gettimeofday(&beginCubic, NULL);
//             // receive data chucks from client
//             int bytesRecived = recvfileChunks(clientSocket, &buffer, sizeof(FILE_SIZE / 2) - sum);
//             sum += bytesRecived;
//             // bzero(buffer, BUFFER_SIZE); add???????
//             if (sum == (FILE_SIZE / 2))
//             {
//                 gettimeofday(&endCubic, NULL);
//                 printf("Half of file bytes are recieved\n");
//                 long seconds = endCubic.tv_sec - beginCubic.tv_sec;
//                 long microsec = endCubic.tv_usec - beginCubic.tv_usec;
//                 elapsedCubic = seconds + microsec * 1e-6;
//                 printf("Time measured for the first part: %f seconds (Cubic session)\n", elapsedCubic);
//             }

//             if (sum != (FILE_SIZE / 2))
//             {
//                 printf("First part of file is not fully sent\n");
//             }

//             printf("So far we have: %d bytes recived.\n", sum);

//             // Send authentication massage to client
//             // authentication maessage - XOR last 4 digits of IDs
//             printf("Sending authentication message to client\n");
//             char authentication[] = "10000010111111";
//             sendToClient(clientSocket, &authentication, sizeof(authentication));

//             // change algorithm to reno
//             printf("Change to Reno method\n");
//             char CCReno[6] = "reno";
//             if (setsockopt(serverSocket, IPPROTO_TCP, TCP_CONGESTION, CCReno, sizeof(CCReno)) != 0)
//             {
//                 printf("Reno transfer failed : %d\n", errno);
//                 return -1;
//             }

//             // measure second part of file with reno algorithm
//             gettimeofday(&beginReno, NULL);
//             if (sum == sizeof(FILE_SIZE))
//             {
//                 gettimeofday(&endReno, NULL);
//                 printf("Whole file bytes are recieved\n");
//                 long secondsReno = endReno.tv_sec - beginReno.tv_sec;
//                 long microsecReno = endReno.tv_usec - beginReno.tv_usec;
//                 elapsedReno = secondsReno + microsecReno * 1e-6;
//                 printf("Time measured for the second part: %f seconds (Reno session)\n", elapsedReno);
//             }

//             if (sum != FILE_SIZE)
//             {
//                 printf("Second part of file is not fully sent\n");
//             }

//             printf("So far we have: %d bytes recived.\n", sum);
//             // restart
//             sum = 0;
//         }

//         // write exit message to quit while loop and print the remain shit
//         char bufferReply[BUFFER_SIZE] = {'\0'};
//         int exitFromSender = recv(clientSocket, bufferReply, BUFFER_SIZE, 0);
//         if (strcmp(bufferReply, "Exit"))
//         {
//             printf("Average of times: \n");
//             return 0;
//         }
//         else
//         {
//             return 1;
//         }
//     }
//     // it means we got an exit message
//     // print times
//     // cubic
//     printf("Cubic: %f\n", elapsedCubic);
//     // reno
//     printf("Reno: %f\n", elapsedReno);
//     // both
//     printf("Total time: \n");
//     // average time
//     // COMPLITE!!!!!!!!!!

//     return 0;
// }

// // backup!!!!!!!!!
// //  // measure time
// //      while (1)
// //      {
// //          // receive data chucks from client
// //          char buffer[maxBuffer];
// //          // memset(buffer, 0, maxBuffer);
// //          gettimeofday(&beginCubic, 0);
// //          int bytesRecived = recvfileChunks(clientSocket, &buffer, sizeof(maxBuffer));
// //          // cubic part
// //          while ((bytesRecived > 0) && sum < (FILE_SIZE / 2))
// //          {
// //              sum += bytesRecived;
// //              // bzero(buffer, BUFFER_SIZE); add???????
// //              if (sum == FILE_SIZE / 2)
// //              {
// //                  gettimeofday(&endCubic, 0);
// //                  long seconds = endCubic.tv_sec - beginCubic.tv_sec;
// //                  long microsec = endCubic.tv_usec - beginCubic.tv_usec;
// //                  elapsedCubic = seconds + microsec * 1e-6;
// //                  printf("Time measured for the first part: %f seconds (Cubic session)\n", elapsedCubic);
// //              }
// //          }

// //         if(sum != (FILE_SIZE / 2))
// //         {
// //             printf("First part of file is not fully sent\n");
// //         }

// //         // Send authentication massage to client
// //         // authentication maessage - XOR last 4 digits of IDs
// //         printf("Sending authentication message to client\n");
// //         char authentication[] = "10000010111111";
// //         sendToClient(clientSocket, &authentication, sizeof(authentication));

// //         // change algorithm to reno
// //         printf("Change to Reno method\n");
// //         char CCReno[6] = "reno";
// //         if (setsockopt(serverSocket, IPPROTO_TCP, TCP_CONGESTION, CCReno, sizeof(CCReno)) != 0)
// //         {
// //             printf("Reno transfer failed : %d\n", errno);
// //             return -1;
// //         }

// //         // measure second part of file with reno algorithm
// //         while (sum > (FILE_SIZE / 2))
// //         {
// //             gettimeofday(&beginReno, 0);
// //             sum += bytesRecived;
// //             if (sum == FILE_SIZE)
// //             {
// //                 gettimeofday(&endReno, 0);
// //                 long secondsReno = endReno.tv_sec - beginReno.tv_sec;
// //                 long microsecReno = endReno.tv_usec - beginReno.tv_usec;
// //                 elapsedReno = secondsReno + microsecReno * 1e-6;
// //                 printf("Time measured for the second part: %f seconds (Reno session)\n", elapsedReno);
// //             }
// //         }

// //         if(sum != FILE_SIZE)
// //         {
// //             printf("Second part of file is not fully sent\n");
// //         }

// //         // write exit message to quit while loop and print the remain shit
// //         char bufferReply[BUFFER_SIZE] = {'\0'};
// //         int exitFromSender = recv(clientSocket, bufferReply, BUFFER_SIZE, 0);
// //         if (strcmp(bufferReply, "Exit"))
// //         {
// //             printf("Average of times: \n");
// //             return 0;
// //         }
// //         else
// //         {
// //             return 1;
// //         }
// //     }
