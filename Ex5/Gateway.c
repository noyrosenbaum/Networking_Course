#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int port;

int serverUDP()
{
    // Create a UDP server socket
    int server;
    if ((server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not create socket : %d", errno);
        return -1;
    }
    int enableReuse = 1;
    int ret = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("Setsockopt() failed with error code : %d\n", errno);
        return 1;
    }
    // setup Server address structure
    struct sockaddr_in serverAddress;
    memset((char *)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    // Bind
    if (bind(server, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("bind() failed with error code : %d", errno);
        return -1;
    }
    printf("After bind(). Waiting for clients");
    return server;
}

int clientUDP()
{
    // Create a UDP client socket
    int client;
    if ((client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not create socket : %d", errno);
        return -1;
    }
    int enableReuse = 1;
    int ret = setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("Setsockopt() failed with error code : %d\n", errno);
        return 1;
    }
    // Setup the server address structure.
    // Port and IP should be filled in network byte order.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port + 1);

    // Bind
    if (bind(client, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("bind() failed with error code : %d", errno);
        return -1;
    }
    return client;
}

int main(int argc, char **argv)
{
    char *port = argv[1];
    char data[IP_MAXPACKET];
    int client = clientUDP();
    int server = serverUDP();
    // setup Client address structure
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset((char *)&clientAddress, 0, sizeof(clientAddress));
    // keep listening for data
    while (1)
    {
        // zero client address
        memset((char *)&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        memset(data, '\0', sizeof(data));
        int recv_len;
        if ((recv_len = recvfrom(server, data, sizeof(data) - 1, 0, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1)
        {
            printf("recvfrom() failed with error code : %d", errno);
            close(server);
            close(client);
            break;
        }
        printf("Received packet\n");
        printf("Data is: %s\n", data);
        float random_num = ((float)random() / (float)RAND_MAX);
        if (random_num > 0.5)
        {
            printf("Random number is: %f\n", random_num);
            printf("Forwards data...\n");
            if (sendto(client, data, sizeof(data), 0, (struct sockaddr *)&clientAddress, clientAddressLen) == -1)
            {
                printf("sendto() failed with error code : %d", errno);
                break;
            }
            else
            {
                printf("doesn't forward data...\n");
            }
        }
    }
    close(client);
    close(server);
    return 0;
}