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

#define RECEIVER_PORT 9999 // The port that the receiver listens
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
        long total = 0;
        while (total != (FILE_SIZE / 2))
        {
            bytesReceived = recv(senderSocket, buffer, BUFFER_SIZE, 0);
            total += bytesReceived;
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
        elapsedCubic = seconds + microsec *1e-6;
        timesofpartone[i - 1] = elapsedCubic;

        // Send authentication message to sender
        // authentication message - bitwise XOR of last 4 digits of our IDs
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
        total = 0;
        while (total != (FILE_SIZE / 2))
        {
            bytesReceived = recv(senderSocket, buffer, BUFFER_SIZE, 0);
            total += bytesReceived;
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

        printf("The second file has been received\n");
        gettimeofday(&endReno, 0);

        seconds = endReno.tv_sec - beginReno.tv_sec;
        microsec = endReno.tv_usec - beginReno.tv_usec;
        elapsedReno = seconds + microsec *1e-6;
        timesofparttwo[i - 1] = elapsedReno;

        char dummyvar = '\0';

        send(senderSocket,&dummyvar,sizeof(char),0);

        printf("Receiver waiting for sender decision\n");

        recv(senderSocket,&dummyvar,sizeof(char),0);
        send(senderSocket,&dummyvar,sizeof(char),0);
        i++;

    }

    close(senderSocket);
    close(listeningSocket);

    int j = 0;

    while (j+1 < i){
        printf("The first part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofpartone[j]);
        avaragepartone += timesofpartone[j];
        printf("The second part of receving massage at iteration number %d took %f seconds\n", j + 1, timesofparttwo[j]);
        avarageparttwo += timesofparttwo[j];
        j++;
    }
    avaragepartone = avaragepartone / --i;
    avarageparttwo = avarageparttwo / i;
    if (i==1)
        printf("There was only one iteration. no avarage time to print\n");
    else{
    printf("The avarge time of receiving part one is: %f\n", avaragepartone);
    printf("The avarge time of receiving part two is: %f\n", avarageparttwo);}
    
    printf("The total avarage time for receiving file is: %f\n", (avaragepartone+avarageparttwo)/2);
    printf("Bye bye:)\n");

    return 0;
}