#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define CLIENT_PORT 8080
#define CLIENT_IP "127.0.0.1"

int main()
{
    int client_sock;
    char buffer[1024];
    int client_sock = socket(AF_INET, SOCK_STREAM, 0); // Create socket - IPv4, TCP, default protocol
    if (client_sock < 0)
        printf("failed to create socket!\n");
    return -1;
    printf("Socket created\n");

    struct sockaddr_in server_addr;
    socklen_t addr_size;

    memset(&server_addr, '\0', sizeof(server_addr)); // clean
    server_addr.sin_family = AF_INET;                // configure details
    server_addr.sin_port = htons(CLIENT_PORT);
    if (inet_pton(AF_INET, CLIENT_IP, &server_addr.sin_addr) <= 0) // server_addr.sin_addr.s_addr = inet_pton(CLIENT_IP);
    {
        printf("Convert was not succeful\n");
        return -1;
    }

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("connection failed :(\n");
        return -1;
    };
    // sent()

    close(client_sock);
    return 0;
}