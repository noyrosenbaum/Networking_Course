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
#include <fcntl.h>

// IPv4 header len without options
#define IP4_HDRLEN 20

// ICMP header len for echo req
#define ICMP_HDRLEN 8

// Make socket a global variable
int sock = -1;

// Checksum algo
unsigned short calculate_checksum(unsigned short *paddress, int len);

// START FROM HERE!!!!!!!!!!!!!!!1
//  run 2 programs using fork + exec
//  command: make clean && make all && ./partb
int main(int argc, char *argv[])
{
    char *args[3];
    if (argc != 3)
    {
        printf("Destination IP parameter is undecleared%d\n", errno);
        exit(1);
    }

    char *destinationIP = argv[2];

    // Create raw socket for IP-RAW (make IP-header by yourself)
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        exit(1);
    }

    // Retrieve current parameter from the socket
    int sockMode = fcntl(sock, F_GETFL, 0);
    if (sockMode < 0)
    {
        printf("fcntl proccess failed with error%d\n", errno);
        exit(1);
    }

    // Set socket to be in non blocking mode for later use using fcntl
    sockMode |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, sockMode) < 0)
    {
        printf("fcntl proccess failed with error%d\n", errno);
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
        // we run in this block whatever is related to watchdog
        printf("in child \n");
        execvp(args[0], args);
    }
    wait(&status); // waiting for child to finish before exiting
    printf("child exit status is: %d", status);

    // here we enter ping code
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

int main(int argc, char *argv[])
{

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
