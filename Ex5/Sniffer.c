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
int counter = 1;
/* Application header */
struct appPacket
{
    uint32_t unixtime;
    uint16_t length;
    uint16_t reserved : 3, c_flag : 1, s_flag : 1, t_flag : 1, status : 10;
    uint16_t cache;
    uint16_t padding;
};

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    
    // while (1)
    // {
    // }
    FILE *file = fopen("206530172_209498211.txt", "w");
    if (file == NULL)
    {
        printf("Can't open file: %d\n", errno);
    }
    // int packet_len = header->len;
    fprintf(file, "Packet number %d:\n", counter);
    counter++;
    // Ethernet
    struct ethhdr *ethernet = (struct ethhdr *)packet;
    // IP
    struct iphdr *ip = (struct iphdr *)(packet + sizeof(struct ethhdr));
    unsigned short iphdrlen = ip->ihl * 4;
    struct sockaddr_in source, dest;
    source.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr = ip->daddr;
    // TCP
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr) + sizeof(struct ethhdr));
    unsigned long tcphdrlen = tcp->doff * 4;
    // App
    struct appPacket *app = (struct appPacket *)(packet + sizeof(struct tcphdr) + sizeof(struct iphdr) + sizeof(struct ethhdr));
    char *data = (char *)(app + sizeof(struct appPacket));
    // TCP payload
    int payloadLen = ntohs(ip->tot_len) - (iphdrlen + tcphdrlen);

    fprintf(file, "\n---------------------------------------------\n");
    fprintf(file, "Source_ip: %s\n", inet_ntoa(source.sin_addr));
    fprintf(file, "Dest_ip: %s\n", inet_ntoa(dest.sin_addr));
    fprintf(file, "Source_port: %u\n", ntohs(tcp->source));
    fprintf(file, "Dest_port: %u\n", ntohs(tcp->dest));
    fprintf(file, "Timestamp: %d\n", htonl(app->unixtime));
    fprintf(file, "Total_length: %d\n", ntohs(app->length));
    fprintf(file, "Cache_flag: %d\n", ntohs(app->c_flag));
    fprintf(file, "Steps_flag: %d\n", ntohs(app->s_flag));
    fprintf(file, "Type_flag: %d\n", ntohs(app->t_flag));
    fprintf(file, "Status_code: %d\n", ntohs(app->status));
    fprintf(file, "Cache_control: %d\n", ntohs(app->cache));
    fprintf(file, "Padding: %d\n", ntohs(app->padding));
    fprintf(file, "Payload:\n");

    for (int i = 0; i < payloadLen; i++)
    {
        fprintf(file, "%02x ", *data);
        data++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            fprintf(file, " ");
    }
    /* print space to handle line less than 8 bytes */
    if (payloadLen < 8)
        fprintf(file, " ");

    /* fill hex gap with spaces if not full line */
    if (payloadLen < 16)
    {
        int gap = 16 - payloadLen;
        for (int i = 0; i < gap; i++)
        {
            fprintf(file, "   ");
        }
    }
    fprintf(file, "   ");

    // for (int i = 0; i < payloadLen + 40; i++)
    // {
    //     fprintf(file, "%02x", (unsigned char)packet[i]);
    //     if (i % 16 == 0)
    //     {
    //         fprintf(file, "\n");
    //     }
    // }
   fprintf(file, "\n---------------------------------------------\n");

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