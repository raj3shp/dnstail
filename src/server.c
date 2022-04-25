#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>

#include "server.h"
#include "dns_protocol.h"

void dns_start(int port, char *host)
{
    int sd;
    struct sockaddr_in sin;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("dnstail: error opening socket");
        exit(1);
    }    
    memset((char *) &sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host);
    sin.sin_port = htons(port);
    if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)))
    {
        perror("dnstail: error binding socket");
        exit(1);
    }
    printf("dnstail: listening on udp %s:%d\n", host, port);
    process_request(sd);
}

void process_request(int sd)
{   
    int recv_size;
    char buf[PACKET_SIZE +4];
    
    struct dns_query *query;
    struct dns_packet *pkt;

    struct sockaddr_in from;
    socklen_t from_addr_size;
    from_addr_size = sizeof(from);

    for(;;)
    {
        recv_size = recvfrom(sd, buf, PACKET_SIZE + 4, 0, (struct sockaddr *)&from, &from_addr_size);
        
        pkt = calloc(1, sizeof(struct dns_packet));
        query = calloc(1, sizeof(struct dns_query));
        
        memcpy(pkt, buf, recv_size);
        
        parse_dns_query(query, recv_size, pkt);
        // convert network byte order to host byte order
        query->src_ip = inet_ntoa((struct in_addr) from.sin_addr);
        query->src_port = ntohs(from.sin_port);
        
        char *type_value;
        type_value = calloc(5, sizeof(char));
        get_query_type_value(query->type, type_value);
        
        printf("dnstail: from:%s:%d, query:%s, type:%s\n", query->src_ip, query->src_port, query->qname, type_value);
        
        // don't forget to cleanup
        free(type_value);
        free(query);
        free(pkt);
    }
}

