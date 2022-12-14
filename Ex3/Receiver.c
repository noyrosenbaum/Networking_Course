#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main()
{
    // Descriptor socket - IPv4, TCP, default protocol
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
        printf("failed to create socket! : %d.\n", errno);
        return -1;
    printf("Socket created\n");

    // Keeping address information of both the server and client
    struct sockaddr_in server_addr, client_addr;
    int client_size = sizeof(client_addr);

    // clean existing data
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // bind the socket to the server address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Bind failed with error code: %d.\n", errno);
    }
    printf("finished binding!\n");

    // Socket listen to incoming calls
    int listener = listen(server_sock, 5);
    printf("Listening...\n");


    // Accept incoming calls
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_size);
    printf("Client connected..!\n");

    char chunk[1024] = {0};
    int receiver = recv(client_sock, chunk, sizeof(BUFFER_SIZE), 0);

    return 0;
}