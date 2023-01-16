#include <pcap.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

/* Application header */
typedef struct calculatorPacket
{
    uint32_t unixtime;
    uint16_t length;
    uint16_t reserved : 3, c_flag : 1, s_flag : 1, t_flag : 1, status : 10;
    uint16_t cache;
    uint16_t padding;
} *pcpack;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    FILE *file = fopen("206530172_209498211.txt", "w");
    if (file == NULL)
    {
        printf("Can't open file: %d\n", errno);
    }
    // Ethernet
    struct ethhdr *ethernet = (struct ethhdr *)packet;
    // IP
    struct iphdr *ip = (struct iphdr *)(packet + sizeof(struct ethhdr));
    struct sockaddr_in source, dest;
    source.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr = ip->daddr;
    // TCP
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr) + sizeof(struct ethhdr));
    // Payload
    pcpack payload = (pcpack)(packet + sizeof(struct tcphdr) + sizeof(struct iphdr) + sizeof(struct ethhdr));
    // time_t unixTime = payload->unixtime;
    // struct time *utc = gmtime(&unixTime);

    fprintf(file, "\n-----------TCP-----------\n");
    fprintf(file, "Source_ip: %s\n", inet_ntoa(source.sin_addr));
    fprintf(file, "Dest_ip: %s\n", inet_ntoa(dest.sin_addr));
    fprintf(file, "Source_port: %u\n", ntohs(tcp->source));
    fprintf(file, "Dest_port: %u\n", ntohs(tcp->dest));
    // fprintf(file, "Timestamp: %s\n", asctime(utc));
    fprintf(file, "Total_length: %d\n", payload->length);
    fprintf(file, "Cache_flag: %d\n", payload->c_flag);
    fprintf(file, "Steps_flag: %d\n", payload->s_flag);
    fprintf(file, "Type_flag: %d\n", payload->t_flag);
    fprintf(file, "Status_code: %d\n", payload->status);
    fprintf(file, "Cache_control: %d\n", payload->cache);
    fprintf(file, "Data: %d\n", payload->padding);
    fprintf(file, "\n-------------------------");

    fclose(file);
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
    char filter_exp[] = "tcp";
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