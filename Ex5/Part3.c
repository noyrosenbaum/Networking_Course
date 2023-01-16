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

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    // Ethernet
    struct ethhdr *ethernet = (struct ethhdr *)packet;
    // IP
    struct iphdr *ip = (struct iphdr *)(packet + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;
    source.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr = ip->daddr;
    if (ip->protocol == IPPROTO_ICMP)
    {
        // char buffer[1500];
        // memset(buffer, 0, 1500);
        // ICMP
        struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));

        if (icmp->type == 8)
        {
            // Calculate the checksum for integrity
            icmp->checksum = 0;
            icmp->checksum = in_cksum((unsigned short *)icmp,
                                      sizeof(struct icmphdr));

            ip->ttl = 115;
            // Swap the src and dest
            ip->saddr = inet_addr("10.0.2.5");
            ip->daddr = inet_addr("1.2.3.4");
        }
        struct sockaddr_in dest_info;
        int enable = 1;

        // Step 1: Create a raw network socket.
        int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

        // Step 2: Set socket option.
        setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
                   &enable, sizeof(enable));

        // Step 3: Provide needed information about destination.
        dest_info.sin_family = AF_INET;
        dest_info.sin_addr.s_addr = ip->daddr;

        // Step 4: Send the packet out.
        sendto(sock, ip, ntohs(ip->tot_len), 0,
               (struct sockaddr *)&dest_info, sizeof(dest_info));
        pclose(sock);
    }
}

int main()
{
    // Handle the sniffed device
    pcap_t *handle;
    // Handles errors
    char errbuf[PCAP_ERRBUF_SIZE];
    // The compiled filter expression
    struct bpf_program fp;
    // Filter expression
    char filter_exp[] = "icmp";
    // IP of our sniffing device
    bpf_u_int32 net;

    // Open live pcap session on NIC with name loopback
    handle = pcap_open_live("lo", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL)
    {
        printf("Can't open device: %s\n", errbuf);
        exit(1);
    }

    // Compile filter_exp into BPF psuedo-code
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
    {
        printf("Can't parse filter: %d\n", errno);
        exit(1);
    }
    // filter specific traffic
    if (pcap_setfilter(handle, &fp) == -1)
    {
        printf("Can't install filter: %d\n", errno);
        exit(1);
    }
    // Capture packets
    pcap_loop(handle, -1, got_packet, NULL);

    pcap_close(handle); // Close the handle
    return 0;
}