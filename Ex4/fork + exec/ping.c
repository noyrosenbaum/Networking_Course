#include <stdio.h>
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
#include <fcntl.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/wait.h>

#define PACKETSIZE 64 // size of data payload
#define SOURCE_IP "127.0.0.1"
#define DESTINATION_IP "127.0.0.1"

// Calculate the checksum of the ICMP header and dataF
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// int pid = -1;
// int loops = 25;
// struct protoent *proto = NULL;

int main()
{
    int rawsocket;
    struct sockaddr_in dest_addr;

    // structs that contains the properties of ICMP protocol
    struct icmphdr icmp;
    // ping message
    char msg[PACKETSIZE - sizeof(struct icmphdr)];

    // Create raw socket
    if (rawsocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP) < 0)
    {
        printf("socket() failed with error:  %d\n", errno);
        exit(1);
    }

    // Set destination address - port is irrelevant
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET; // IPv4
    dest_addr.sin_addr.s_addr = inet_addr(DESTINATION_IP);
    // if (inet_pton(AF_INET, DESTINATION_IP, &dest_addr.sin_addr) <= 0)
    // {
    //     printf("inet_pton() failed with error:  %d\n", errno);
    //     exit(1);
    // }

    // ICMP header fields
    icmp.type = ICMP_ECHO; //ping massage definition

    return 0;
}
