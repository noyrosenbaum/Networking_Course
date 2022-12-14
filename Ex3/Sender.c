#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define CLIENT_PORT 8999
#define CLIENT_IP "127.0.0.1"

int main()
{
    int client_sock;
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0)
        return -1;
    printf("Socket created\n");

    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];

    memset(&addr, "\0", sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = CLIENT_PORT;
    addr.sin_addr.s_addr = inet_addr(CLIENT_IP);

    connect(client_sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to the server");
    return 0;
}