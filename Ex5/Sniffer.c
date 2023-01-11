#include <pcap.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

typedef struct calculatorPacket
{
    uint32_t unixtime;
    uint16_t length;
    uint16_t reserved : 3, c_flag : 1, s_flag : 1, t_flag : 1, status : 10;
    uint16_t cache;
    uint16_t padding;
} cpack, *pcpack;

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

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
    char filter_exp[] = "ip proto icmp";
    // IP of our sniffing device
    bpf_u_int32 net;

    // Open live pcap session on NIC with name eth3
    handle = pcap_open_live("eth3", BUFSIZ, 1, 1000, errbuf);
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