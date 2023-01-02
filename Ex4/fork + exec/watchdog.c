#include <stdio.h>
#include <sys/time.h>
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
#include <fcntl.h>

#define TIMEOUT 10 // milliseconds - ENSURE IN CODE THE TIME UNITS
#define RECEIVER_PORT 3000
#define IP "127.0.0.1"

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

    // Retrieve current parameter from the socket
    int sockMode = fcntl(listeningSocket, F_GETFL, 0);
    if (sockMode < 0)
    {
        printf("fcntl proccess failed with error%d\n", errno);
        exit(1);
    }

    // Set socket to be in non blocking mode for later use using fcntl
    sockMode |= O_NONBLOCK;
    if (fcntl(listeningSocket, F_SETFL, sockMode) < 0)
    {
        printf("Set non-blocking I/O mode failed with error%d\n", errno);
        exit(1);
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6

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
    return 0;
}

// Staff's code
/*
    int timer = 0;
    while (timer < 10)
    {
        recv();
        timer = 0seconds;
    }
    send("timeout")
*/