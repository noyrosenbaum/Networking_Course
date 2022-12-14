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
#define CLIENT_PORT 8080
#define CLIENT_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define CHUNK 1024 // Read 1024 bytes at a time

void sendToReceiver(int socket)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char buf[CHUNK];
    // Read 1MB file and sent it to Receiver
    size_t file_size;
    FILE *message;
    message = fopen("test.txt", "r");

    while ((file_size = fread(buf, 1, sizeof(buf), message)) > 0)
    {
        int bytesSent = send(socket, buf, CHUNK, 0);
        if (bytesSent == -1)
        {
            printf("send() failed with error code : %d.\n", errno);
        }
        else if (bytesSent == 0)
        {
            printf("peer has closed the TCP connection prior to send().\n");
        }
        else if (bytesSent < file_size)
        {
            printf("sent only %ld bytes from the required %d.\n", file_size, bytesSent);
        }
        else
        {
            printf("message was successfully sent.\n");
        }
    }
    fclose(message);
}

int main()
{
    // Create socket - IPv4, TCP, default protocol
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0)
    {
        printf("failed to create socket! : %d.\n", errno);
        return -1;
    }
    printf("Socket created\n");

    struct sockaddr_in server_addr;
    // clean
    memset(&server_addr, '\0', sizeof(server_addr));
    // configure details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CLIENT_PORT);
    if (inet_pton(AF_INET, CLIENT_IP, &server_addr.sin_addr) <= 0)
    {
        printf("inet_pton() failed\n");
        return -1;
    }

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("connection failed with error code : %d.\n", errno);
        close(client_sock);
        return -1;
    };

    printf("Connected to server\n");
    // Send data away
    sendToReceiver(client_sock);
    close(client_sock);

    return 0;
}