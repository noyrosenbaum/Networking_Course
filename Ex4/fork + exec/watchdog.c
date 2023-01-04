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

#define TIMEOUT 10 // milliseconds - ENSURE IN CODE THE TIME UNITS
#define SERVER_PORT 3000
#define SERVER_IP "127.0.0.1"

int createSocket(struct sockaddr_in *serverAddress)
{
    // Open the listening (server) socket
    int listeningSocket = -1;
    listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
    if (listeningSocket == -1)
    {
        printf("Could not create listening socket : %d\n", errno);
        return 1;
    }

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d\n", errno);
        return 1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    // struct sockaddr_in serverAddress;
    memset(serverAddress, 0, sizeof(*serverAddress));

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_addr.s_addr = INADDR_ANY;  // any IP at this port (Address to accept any incoming messages)
    serverAddress->sin_port = htons(SERVER_PORT); // network order (makes byte order consistent)

    // Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningSocket, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
    if (bindResult == -1)
    {
        printf("Bind failed with error code : %d\n", errno);
        // close the socket
        close(listeningSocket);
        exit(1);
    }
    else
    {
        printf("Bind() success\n");
    }

    // Make the socket listening; actually mother of all client sockets.
    // 500 is a Maximum size of queue connection requests
    // number of concurrent connections
    int listenResult = listen(listeningSocket, 300);
    if (listenResult == -1)
    {
        printf("listen() failed with error code : %d\n", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }
    else
    {
        printf("Listening for connections on port %d...\n", SERVER_PORT);
    }

    return listeningSocket;
}

int main()
{
    // Create Client socket
    int serverSocket;
    struct sockaddr_in serverAddress;
    serverSocket = createSocket(&serverAddress);

    // Build Client information
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);

    // Accepts incoming connections
    int senderSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (senderSocket == -1)
    {
        printf("Accept failed with error code : %d\n", errno);
        // close the sockets
        close(serverSocket);
        return -1;
    }
    else
    {
        printf("Client connected..!\n");
    }

    // timer section
    int timer = 0;
    char signal[1] = {0};
    int signalRecv = recv(senderSocket, signal, sizeof(signal), MSG_DONTWAIT);
    while (timer < 10)
    {
        timer++;
        sleep(1);
        if (timer == 10)
        {
            break; // out of the loop
        }
        if (signalRecv > 0)
        {
            timer = 0;
        }
    }

    printf("server %s cannot be reached.\n", SERVER_IP);
    close(serverSocket);
    close(senderSocket);
    kill(getppid(), SIGKILL);
    return 0;
}
