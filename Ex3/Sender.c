#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

int main()
{
    int client_sock;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
        return -1;
    printf("Socket created\n");

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

    memset(&server_addr, "\0", sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int binder = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    int listener = listen(server_sock, 5);
    printf("Listening...\n");

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        printf("Client connected..!\n");
    }
    



    // int yes = 1;
    // If(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    // {
    //     perror("setsockopt");
    //     exit(1);
    // }

    return 0;
}
