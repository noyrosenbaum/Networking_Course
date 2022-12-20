// Linux and other UNIXes
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5060 // The port that the server listens
#define BUFFER_SIZE 1024
#define CHUNK 1024 // Read 1024 bytes at a time
#define FILE_SIZE 1060424
static double totalTimes = 0;
int maxBuffer = 1024;

int createSocket(struct sockaddr_in *serverAddress)
{
    // Open the listening (server) socket
    int listeningSocket = -1;
    listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
    if (listeningSocket == -1)
    {
        printf("Could not create listening socket : %d", errno);
        return 1;
    }

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    //
    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        return 1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_addr.s_addr = INADDR_ANY;  // any IP at this port (Address to accept any incoming messages)
    serverAddress->sin_port = htons(SERVER_PORT); // network order (makes byte order consistent)

    // Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        printf("Bind failed with error code : %d", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    // 500 is a Maximum size of queue connection requests
    // number of concurrent connections
    int listenResult = listen(listeningSocket, 3);
    if (listenResult == -1)
    {
        printf("listen() failed with error code : %d", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }
    printf("Sender's socket successfully created\n");
    return listeningSocket;
}

// Recieve chunks of file - when inserting parameters to function - we write the pointer of the wanted value
int recvfileChunks(int cSocket, void *buffer, int maxBuffer)
{
    int bytesReceived = recv(cSocket, buffer, maxBuffer, 0);
    if (bytesReceived == -1)
    {
        printf("recv failed with error code : %d", errno);
        close(cSocket);
        return -1;
    }

    printf("Received: %d", buffer);
    return bytesReceived;
}

//Measure time
// int measureTime()

// send to client
int sendToClient(int cSocket, void *buffer, int maxBuffer)
{
    int bytesSend = send(cSocket, buffer, maxBuffer, 0);
    if (bytesSend == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(cSocket);
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
    int serverSocket;
    struct sockaddr_in serverAddress;
    serverSocket = createSocket(&serverAddress);
    printf("Listen to client\n");

    // Build Client information
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);

    // Accepts incoming connections
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (clientSocket == -1)
    {
        printf("listen failed with error code : %d", errno);
        // close the sockets
        close(serverSocket);
        return -1;
    }
    printf("A new client connection accepted\n");

    // receive data chucks from client
    char buffer[maxBuffer];
    memset(buffer, 0, maxBuffer);
    int bytesRecived = recvfileChunks(clientSocket, &buffer, sizeof(maxBuffer));

    // sum the bytes so it will not pass half of file size
    int sum = 0;
    struct timeval beginCubic, endCubic;
    struct timeval beginReno, endReno;

    // ADD CONDIION WHICH DIFFER THE ALGOROTHMS BELOW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    while ((bytesRecived > 0) && sum < (FILE_SIZE / 2))
    {
        gettimeofday(&beginCubic, 0);
        sum += bytesRecived;
        // bzero(buffer, BUFFER_SIZE); add???????
        if (sum == FILE_SIZE / 2)
        {
            gettimeofday(&endCubic, 0);
            long seconds = endCubic.tv_sec - beginCubic.tv_sec;
            long microsec = endCubic.tv_usec - beginCubic.tv_usec;
            double elapsed = seconds + microsec * 1e-6;
            printf("Time measured for the first part: %f seconds (Cubic session)\n", elapsed);
        }
        else
        {
            printf("First part of file is not fully sent\n");
        }
    }

    // Send authentication massage to client
    // authentication maessage - XOR last 4 digits of IDs
    printf("Sending authentication message to client\n");
    char authentication[] = "10000010111111";
    sendToClient(clientSocket, &authentication, sizeof(authentication));

    return 0;
}