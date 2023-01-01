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

#define RECEIVER_PORT 9999
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
    int senderSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
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