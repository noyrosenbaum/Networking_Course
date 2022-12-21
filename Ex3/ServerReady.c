// /*
//     TCP/IP-server
// */

// #include <stdio.h>

// // Linux and other UNIXes
// #include <errno.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <signal.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>

// #define SERVER_PORT 5060 // The port that the server listens
// #define BUFFER_SIZE 1024
// #define CHUNK 1024 // Read 1024 bytes at a time
// #define FILE_SIZE 1060424
// static double totalTimes = 0;

// int acceptAndRecieveSocket(int socket)
// {
//     // Accept and incoming connection
//     printf("Waiting for incoming TCP-connections...\n");
//     struct sockaddr_in clientAddress;
//     socklen_t clientAddressLen = sizeof(clientAddress);
//     //authentication maessage - XOR last 4 digits of IDs
//     char authentication[] = "10000010111111";

//     while (1)
//     {
//         memset(&clientAddress, 0, sizeof(clientAddress));
//         clientAddressLen = sizeof(clientAddress);
//         int clientSocket = accept(socket, (struct sockaddr *)&clientAddress, &clientAddressLen);
//         if (clientSocket == -1)
//         {
//             printf("listen failed with error code : %d", errno);
//             // close the sockets
//             close(socket);
//             return -1;
//         }

//         printf("A new client connection accepted\n");

//         // Receive chunks of data from client
//         char buffer[BUFFER_SIZE];
//         memset(buffer, 0, BUFFER_SIZE);
//         int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
//         int sum = 0;
//         while (bytesReceived > 0 && sum < FILE_SIZE / 2)
//         {
//             sum += bytesReceived;
//             bzero(buffer, BUFFER_SIZE);
//         }
//         if (bytesReceived == -1)
//         {
//             printf("recv failed with error code : %d", errno);
//             // close the sockets
//             close(socket);
//             close(clientSocket);
//             return -1;
//         }

//         printf("Received: %d", sum);

//         //get exit message from server
//         // char *exitMessage = "exit";
//         // int messageLen = strlen(exitMessage) + 1;
//         // int bytesSentExit = recv(clientSocket, exitMessage, messageLen, 0);
        


//         //send authentication to client
//         int bytesSent = send(clientSocket, authentication, sizeof(authentication), 0);
//         if (bytesSent == -1)
//         {
//             printf("send() failed with error code : %d", errno);
//             close(socket);
//             close(clientSocket);
//             return -1;
//         }
//         else if (bytesSent == 0)
//         {
//             printf("peer has closed the TCP connection prior to send().\n");
//         }
//         else if (bytesSent < sizeof(authentication))
//         {
//             printf("sent only %d bytes from the required %d.\n", sizeof(authentication), bytesSent);
//         }
//         else
//         {
//             printf("message was successfully sent.\n");
//         }
//     }
// }

// int main()
// {
//     // signal(SIGPIPE, SIG_IGN);  // on linux to prevent crash on closing socket

//     // Open the listening (server) socket
//     int listeningSocket = -1;
//     listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
//     if (listeningSocket == -1)
//     {
//         printf("Could not create listening socket : %d", errno);
//         return 1;
//     }

//     // Reuse the address if the server socket on was closed
//     // and remains for 45 seconds in TIME-WAIT state till the final removal.
//     //
//     int enableReuse = 1;
//     int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
//     if (ret < 0)
//     {
//         printf("setsockopt() failed with error code : %d", errno);
//         return 1;
//     }

//     // "sockaddr_in" is the "derived" from sockaddr structure
//     // used for IPv4 communication. For IPv6, use sockaddr_in6
//     //
//     struct sockaddr_in serverAddress;
//     memset(&serverAddress, 0, sizeof(serverAddress));

//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = INADDR_ANY;  // any IP at this port (Address to accept any incoming messages)
//     serverAddress.sin_port = htons(SERVER_PORT); // network order (makes byte order consistent)

//     // Bind the socket to the port with any IP at this port
//     int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
//     if (bindResult == -1)
//     {
//         printf("Bind failed with error code : %d", errno);
//         // close the socket
//         close(listeningSocket);
//         return -1;
//     }

//     printf("Bind() success\n");

//     // Make the socket listening; actually mother of all client sockets.
//     // 500 is a Maximum size of queue connection requests
//     // number of concurrent connections
//     int listenResult = listen(listeningSocket, 3);
//     if (listenResult == -1)
//     {
//         printf("listen() failed with error code : %d", errno);
//         // close the socket
//         close(listeningSocket);
//         return -1;
//     }

//     // measure time of first arrival with Cubic
//     int j = 0;
//     while (j++ < 5)
//     {
//         struct timeval begin, end;
//         gettimeofday(&begin, 0);
//         acceptAndRecieveSocket(listeningSocket);
//         gettimeofday(&end, 0);
//         long seconds = end.tv_sec - begin.tv_sec;
//         long microsec = end.tv_usec - begin.tv_usec;
//         double elapsed = seconds + microsec * 1e-6;
//         printf("Time measured for the first part: %f seconds (Cubic session)\n", elapsed);
//     }

//     // change to reno
//     printf("Change to Reno method\n");
//     char BUF[BUFFER_SIZE];
//     strcpy(BUF, "reno");
//     if (setsockopt(listeningSocket, IPPROTO_TCP, TCP_CONGESTION, BUF, sizeof(BUF)) != 0)
//     {
//         perror("setsockopt");
//         return -1;
//     }

//     // measure time of second arrival with Reno
//     int i = 0;
//     while (i++ < 5)
//     {
//         struct timeval beginReno, endReno;
//         gettimeofday(&beginReno, 0);
//         acceptAndRecieveSocket(listeningSocket);
//         gettimeofday(&endReno, 0);
//         long secondsReno = endReno.tv_sec - beginReno.tv_sec;
//         long microsecReno = endReno.tv_usec - beginReno.tv_usec;
//         double elapsedReno = secondsReno + microsecReno * 1e-6;
//         printf("Time measured for the second part: %f seconds (Reno session)/n", elapsedReno);
//     }

//     // receive message from server

//     // double totalTimes = elapsed + elapsedReno;
//     // printf("Total time of recieving file is: %f", totalTimes);

//     close(listeningSocket);
//     return 0;
// }
