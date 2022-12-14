#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

int main()
{
    int client_sock;
    char buffer[1024];
    int server_sock = socket(AF_INET, SOCK_STREAM, 0); //descriptor socket
    if (server_sock < 0)
        printf("failed to create socket!\n");
        return -1;
    printf("Socket created\n");

    struct sockaddr_in server_addr, client_addr; //Keeping address information of both the server and client
    socklen_t client_size;
    
    memset(&server_addr, '\0', sizeof(server_addr)); //clean existing data 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int binder = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); //bind the socket to the server address
    printf("finished binding!\n");

    int listener = listen(server_sock, 5); //Socket listen to incoming calls 
    printf("Listening...\n");

    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size); //Accept incoming calls
    printf("Client connected..!\n");
    
    int receiver = recv(client_sock, client_size, strlen(buffer), 0);
 
    close(client_sock);
    close(server_sock);

    return 0;
}