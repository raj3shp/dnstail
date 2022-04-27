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

#include "dns_protocol.h"

#define NS_TIMEOUT 5
#define NAMESERVER "8.8.8.8"

void proxy_dns_reuqest(char *req_buf, int req_size, struct sockaddr_in from, int sd)
{
    struct sockaddr_in sin;
    int sin_len = sizeof(sin);
    int ns_sock;
    char buf[PACKET_SIZE + 4];
    char *send_buf, *res_buf;
    int recv_size;
    struct timeval tv;

    socklen_t from_addr_size;
    from_addr_size = sizeof(from);

    struct dns_query *query;
    struct dns_packet *pkt;

    send_buf = malloc(PACKET_SIZE + 4);
    res_buf = malloc(PACKET_SIZE + 4);

    memcpy(send_buf, req_buf, req_size);

    char *type_value;
    type_value = calloc(5, sizeof(char));

    tv.tv_sec = NS_TIMEOUT;
    tv.tv_usec = 0;

    ns_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(ns_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(53);
    sin.sin_addr.s_addr = inet_addr(NAMESERVER);
    sendto(ns_sock, send_buf, req_size, 0, (struct sockaddr *)&sin, sin_len);
    recv_size = recv(ns_sock, buf, 255, 0);
    close(ns_sock);

    if (recv_size > 0)
    {
        memcpy(res_buf, buf, recv_size);
        sendto(sd, res_buf, 255, 0, (struct sockaddr *)&from, from_addr_size);
        pkt = calloc(1, sizeof(struct dns_packet));
        query = calloc(1, sizeof(struct dns_query));

        memcpy(pkt, buf, recv_size);
        parse_dns_query(query, recv_size, pkt);
        // convert network byte order to host byte order
        query->src_ip = inet_ntoa((struct in_addr)from.sin_addr);
        query->src_port = ntohs(from.sin_port);

        get_query_type_value(query->type, type_value);

        printf("dnstail: from:%s:%d, query:%s, type:%s\n", query->src_ip, query->src_port, query->qname, type_value);
        free(query);
        free(pkt);
    }
    free(send_buf);
    free(res_buf);
    free(type_value);

    // make sure to always exit
    exit(0);
}