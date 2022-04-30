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

int forward_to_ns(char *req_buf, int req_size, char *ns_res)
{
    int sock, recv_size;

    struct timeval tv;
    struct sockaddr_in sin;

    int sin_len = sizeof(sin);
    tv.tv_sec = NS_TIMEOUT;
    tv.tv_usec = 0;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv); // set timeout
    memset((char *)&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(53);
    sin.sin_addr.s_addr = inet_addr(NAMESERVER);
    sendto(sock, req_buf, req_size, 0, (struct sockaddr *)&sin, sin_len);
    recv_size = recv(sock, ns_res, 255, 0);
    close(sock);

    return recv_size;
}

void proxy_dns_reuqest(char *req_buf, int req_size, struct sockaddr_in from, int sd)
{
    int recv_size;
    char *res_buf, *type_value;
    struct dns_query *query;
    socklen_t from_addr_size;

    from_addr_size = sizeof(from);
    res_buf = malloc(PACKET_SIZE + 4);
    type_value = calloc(5, sizeof(char));

    // resolve from ns
    recv_size = forward_to_ns(req_buf, req_size, res_buf);

    if (recv_size > 0)
    {
        // forward reply to client
        sendto(sd, res_buf, 255, 0, (struct sockaddr *)&from, from_addr_size);
        
        query = calloc(1, sizeof(struct dns_query));
        parse_dns_query(query, req_buf, recv_size);

        query->src_ip = inet_ntoa((struct in_addr)from.sin_addr);
        query->src_port = ntohs(from.sin_port);
        
        get_query_type_value(query->type, type_value);
        
        printf("dnstail: from:%s:%d, query:%s, type:%s\n", query->src_ip, query->src_port, query->qname, type_value);
        free(query);
    }
    free(res_buf);
    free(type_value);
}
