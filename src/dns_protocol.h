#ifndef __DNS_PROTOCOL_H__
#define __DNS_PROTOCOL_H__

#include <stdlib.h>
#define PACKET_SIZE 512
#define MAX_QUERY_SIZE 253

struct dns_header
{
    u_int16_t id;
    u_int16_t qr:1;
    u_int16_t opcode:4;
    u_int16_t aa:1;
    u_int16_t tc:1;
    u_int16_t rd:1;
    u_int16_t ra:1;
    u_int16_t rcode:4;
    u_int16_t qdcount;
    u_int16_t ancount;
    u_int16_t nscount;
    u_int16_t arcount;
};

struct dns_packet
{
    struct dns_header header;
    char data[PACKET_SIZE];
};

struct dns_query
{
    char qname[MAX_QUERY_SIZE];
    u_int16_t type, src_port;
    char *src_ip;
};

void parse_dns_query(struct dns_query *query, int query_size, struct dns_packet *pkt);
void get_query_type_value(u_int16_t type, char *value);

#endif