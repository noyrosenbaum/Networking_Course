#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> // gettimeofday()
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_PORT 3000
#define SERVER_IP "127.0.0.1"

// IPv4 header len without options
#define IP4_HDRLEN 20

// ICMP header len for echo req
#define ICMP_HDRLEN 8

// Make socket a global variable
int sock = -1;

// Checksum algo
unsigned short calculate_checksum(unsigned short *paddress, int len);

// Craete new socket
int createSocket(struct sockaddr_in *serverAddress)
{
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1)
    {
        printf("Could not create socket : %d\n", errno);
        return -1;
    }

    memset(serverAddress, 0, sizeof(*serverAddress));

    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(SERVER_PORT);                                     // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP, &serverAddress->sin_addr); // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0)
    {
        printf("inet_pton() failed\n");
        return -1;
    }
    return client_sock;
}

// START FROM HERE!!!!!!!!!!!!!!!1
//  run 2 programs using fork + exec
//  command: make clean && make all && ./partb
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Destination IP parameter is undecleared%d\n", errno);
        exit(1);
    }

    char *destinationIP = argv[1];
    char *args[2];
    // Create raw socket for IP-RAW
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() (raw) failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        exit(1);
    }

    // Set Time-To-Live (TTL) to 115
    int ttl = 115;
    if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
    {
        printf("setsockopt() failed with error %d\n", errno);
        exit(1);
    }

    // compiled watchdog.c by makefile
    args[0] = "./watchdog";
    args[1] = NULL;
    int status;
    int pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
    }
    sleep(1);

    // Create TCP socket
    int clientSocket;
    struct sockaddr_in serverAddress;
    clientSocket = createSocket(&serverAddress);

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d\n", errno);
        // cleanup the socket;
        close(clientSocket);
        return -1;
    }

    printf("connected to server\n");

    struct icmp icmphdr; // ICMP-header
    char data[IP_MAXPACKET] = "This is the ping.\n";
    int datalen = strlen(data) + 1;

    // Sequence Number (16 bits): starts at 0
    unsigned short seq = icmphdr.icmp_seq;
    seq = 0;

    printf("PING %s: %d data bytes\n", destinationIP, datalen);

    while (1)
    {
        //===================
        // ICMP header
        //===================

        // Message Type (8 bits): ICMP_ECHO_REQUEST
        icmphdr.icmp_type = ICMP_ECHO;

        // Message Code (8 bits): echo request
        icmphdr.icmp_code = 0;

        // Identifier (16 bits): some number to trace the response.
        // It will be copied to the response packet and used to map response to the request sent earlier.
        // Thus, it serves as a Transaction-ID when we need to make "ping"
        icmphdr.icmp_id = 18;

        // ICMP header checksum (16 bits): set to 0 not to include into checksum calculation
        icmphdr.icmp_cksum = 0;

        // Combine the packet
        char packet[IP_MAXPACKET];

        // Next, ICMP header
        memcpy((packet), &icmphdr, ICMP_HDRLEN);

        // After ICMP header, add the ICMP data.
        memcpy(packet + ICMP_HDRLEN, data, datalen);

        // Calculate the ICMP header checksum
        icmphdr.icmp_cksum = calculate_checksum((unsigned short *)(packet), ICMP_HDRLEN + datalen);
        memcpy((packet), &icmphdr, ICMP_HDRLEN);

        struct sockaddr_in dest_in;
        memset(&dest_in, 0, sizeof(struct sockaddr_in));
        dest_in.sin_family = AF_INET;

        // The port is irrelant for Networking and therefore was zeroed.
        dest_in.sin_addr.s_addr = inet_addr(destinationIP);

        struct timeval start, end;
        gettimeofday(&start, 0);

        // Send the packet using sendto() for sending datagrams.
        int bytes_sent = sendto(sock, packet, ICMP_HDRLEN + datalen, 0, (struct sockaddr *)&dest_in, sizeof(dest_in));
        if (bytes_sent == -1)
        {
            fprintf(stderr, "sendto() failed with error: %d\n", errno);
            return -1;
        }

        // alert watchdog to set timer after sending a message
        char signal[1] = {1};
        int signalSend = send(clientSocket, signal, sizeof(signal), 0);
        if (signalSend == -1)
        {
            printf("Send() failed with error code : %d\n", errno);
            close(clientSocket);
            return -1;
        }
        else if (signalSend == 0)
        {
            printf("Peer has closed the TCP connection prior to send().\n");
            return -1;
        }
        // Get the ping response
        bzero(packet, IP_MAXPACKET);
        socklen_t len = sizeof(dest_in);
        ssize_t bytes_received = -1;

        while ((bytes_received = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_in, &len)))
        {
            if (bytes_received > 0)
            {
                // Check the IP header
                struct iphdr *iphdr = (struct iphdr *)packet;
                struct icmphdr *icmphdr = (struct icmphdr *)(packet + (iphdr->ihl * 4));
                break;
            }
        }

        gettimeofday(&end, 0);

        char reply[IP_MAXPACKET];
        memcpy(reply, packet + ICMP_HDRLEN + IP4_HDRLEN, datalen);
        float milliseconds = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;

        // Format acceptence massage
        printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%f ms\n", bytes_received, destinationIP, seq, ttl, milliseconds);
        seq++;
        sleep(1);
    }
    // Close the raw socket descriptor.
    close(sock);
    return 0;
}

// Compute checksum (RFC 1071).
unsigned short calculate_checksum(unsigned short *paddress, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = paddress;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    answer = ~sum;                      // truncate to 16 bits

    return answer;
}