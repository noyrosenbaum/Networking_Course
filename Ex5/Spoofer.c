#include <pcap.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <net/ethernet.h> // Ethernet header details
#include <netinet/ether.h>
#include <netinet/tcp.h>     // TCP header details
#include <netinet/udp.h>     // UDP header details
#include <netinet/ip.h>      // IP header details
#include <netinet/ip_icmp.h> // ICMP header details
char *newSourceIP;
char *destinationIP;
int send_raw_ip_packet(struct iphdr *ip);

unsigned short in_cksum(unsigned short *buf, int length)
{
    unsigned short *w = buf;
    int nleft = length;
    int sum = 0;
    unsigned short temp = 0;

    /*
     * The algorithm uses a 32 bit accumulator (sum), adds
     * sequential 16 bit words to it, and at the end, folds back all
     * the carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    /* treat the odd byte at the end, if any */
    if (nleft == 1)
    {
        *(u_char *)(&temp) = *(u_char *)w;
        sum += temp;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
    sum += (sum >> 16);                 // add carry
    return (unsigned short)(~sum);
}

int main(int argc, char *argv[])
{
    destinationIP = argv[1];
    newSourceIP = argv[2];
    char buffer[IP_MAXPACKET];
    memset(buffer, 0, IP_MAXPACKET);
    // Ethernet
    // struct ethhdr *ethernet = (struct ethhdr *)buffer;

    /*********************************************************
       Step 1: Fill in the ICMP header.
     ********************************************************/
    // ICMP
    struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));

    icmp->type = ICMP_ECHO; // ICMP Type: 8 is request, 0 is reply.

    // Calculate the checksum for integrity
    icmp->checksum = 0;
    icmp->checksum = in_cksum((unsigned short *)icmp,
                              sizeof(struct icmphdr));

    /*********************************************************
       Step 2: Fill in the IP header.
     ********************************************************/
    // IP
    struct iphdr *ip = (struct iphdr *)buffer;
    ip->version = 4;
    ip->ihl = 5;
    ip->ttl = 20;
    ip->saddr = inet_addr(newSourceIP);
    ip->daddr = inet_addr(destinationIP);
    ip->protocol = IPPROTO_ICMP;
    ip->tot_len = htons(sizeof(struct iphdr) +
                        sizeof(struct icmphdr));

    /*********************************************************
       Step 3: Finally, send the spoofed packet
     ********************************************************/
    send_raw_ip_packet(ip);

    return 0;
}

int send_raw_ip_packet(struct iphdr *ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Step 1: Create a raw network socket.
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock == -1)
    {
        fprintf(stderr, "socket() failed with error: %d", errno);
        fprintf(stderr, "To create a raw socket, the process needs to be run by Admin/root user.\n\n");
        exit(1);
    }

    // Step 2: Set socket option.
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0)
    {
        printf("setsockopt() failed with error %d\n", errno);
        exit(1);
    }

    // Step 3: Provide needed information about destination.
    memset(&dest_info, 0, sizeof(struct sockaddr_in));
    dest_info.sin_family = AF_INET;
    dest_info.sin_addr.s_addr = ip->daddr;

    // Step 4: Send the packet out.
    if (sendto(sock, ip, ntohs(ip->tot_len), 0, (struct sockaddr *)&dest_info, sizeof(dest_info)) == -1)
    {
        fprintf(stderr, "sendto() failed with error: %d\n", errno);
        return -1;
    }
    printf("Sent a packet from fake source IP: %s to destination: %s\n", newSourceIP, destinationIP);
    close(sock);
}