#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main()
{
    int client_sock;
    //descriptor socket - IPv4, TCP, default protocol
    int server_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_sock < 0)
        perror("failed to create socket!\n");
        return -1;
    printf("Socket created\n");

    //Keeping address information of both the server and client
    struct sockaddr_in server_addr, client_addr; 
    socklen_t client_size;
    
    //clean existing data
    memset(&server_addr, '\0', sizeof(server_addr));  
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //bind the socket to the server address
    int binder = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); 
    printf("finished binding!\n");

    //Socket listen to incoming calls
    int listener = listen(server_sock, 5);  
    printf("Listening...\n");

    client_size = sizeof(client_addr);
    //Accept incoming calls
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size); 
    printf("Client connected..!\n");
    
    int receiver = recv(client_sock, client_size, strlen(BUFFER_SIZE), 0);
 
    close(client_sock);
    close(server_sock);

    return 0;
}