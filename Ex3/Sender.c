#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define CLIENT_PORT 8080
#define CLIENT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main()
{
    // Read 1MB file
    FILE *message = fopen("test.txt", "r");
    if (message == NULL)
    {
        perror("File is Empty\n");
        return -1;
    }


    int client_sock;
    // Create socket - IPv4, TCP, default protocol
    int client_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (client_sock < 0)
        printf("failed to create socket!\n");
    return -1;
    printf("Socket created\n");

    struct sockaddr_in server_addr;
    socklen_t addr_size;

    // clean
    memset(&server_addr, '\0', sizeof(server_addr));
    // configure details 
    server_addr.sin_family = AF_INET;                
    server_addr.sin_port = htons(CLIENT_PORT);
    if (inet_pton(AF_INET, CLIENT_IP, &server_addr.sin_addr) <= 0) 
    {
        perror("Convert was not succeful\n");
        return -1;
    }

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connection failed :(\n");
        return -1;
    };
    // sent()

    close(client_sock);
    fclose(message);
    return 0;
}