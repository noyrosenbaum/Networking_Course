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
    // int sock = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP connection, IP
    //  int connectResult = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // if (connectResult == -1) {
    //     return -1;
    // }

    // // Set port and IP of server
    // struct sockaddr_in server_addr;
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(CLIENT_PORT);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    return 0;
}
