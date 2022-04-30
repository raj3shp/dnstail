#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "dns_protocol.h"

void parse_dns_query(struct dns_query *dns_qr, char *req_buf, int req_size)
{
    u_int16_t i, j, type;
    char *fqdn, *question;

    struct dns_packet *pkt;

    pkt = calloc(1, sizeof(struct dns_packet));
    memcpy(pkt, req_buf, req_size);

    question = pkt->data;

    // max domain name size 253 characters
    fqdn = calloc(MAX_QUERY_SIZE, sizeof(char));

    j = 0;
    while (j <= req_size)
    {
        int length = question[j];
        // length 0 indicates end of query
        // 63 chars max namspace length
        if (length == 0 || length > 63)
            break;
        for (i = 1; i <= length; i++)
        {
            fqdn[(i - 1) + j] = question[i + j];
        }
        strcat(fqdn, ".");
        j += (length + 1);
    }
    int query_len = strlen(fqdn);
    type = question[query_len + 2];
    strncpy(dns_qr->qname, fqdn, query_len);
    dns_qr->type = type;

    free(fqdn);
    free(pkt);
}

void get_query_type_value(u_int16_t type, char *value)
{
    char id;
    switch (type)
    {
    case 1:
        strcpy(value, "A");
        break;
    case 2:
        strcpy(value, "NS");
        break;
    case 3:
        strcpy(value, "MD");
        break;
    case 4:
        strcpy(value, "MF");
        break;
    case 5:
        strcpy(value, "CNAME");
        break;
    case 6:
        strcpy(value, "SOA");
        break;
    case 7:
        strcpy(value, "MB");
        break;
    case 8:
        strcpy(value, "MG");
        break;
    case 9:
        strcpy(value, "MR");
        break;
    case 10:
        strcpy(value, "NULL");
        break;
    case 11:
        strcpy(value, "WKS");
        break;
    case 12:
        strcpy(value, "PTR");
        break;
    case 13:
        strcpy(value, "HINFO");
        break;
    case 14:
        strcpy(value, "MINFO");
        break;
    case 15:
        strcpy(value, "MX");
        break;
    case 16:
        strcpy(value, "TXT");
        break;
    case 28:
        strcpy(value, "AAAA");
        break;
    case 252:
        strcpy(value, "AXFR");
        break;
    case 253:
        strcpy(value, "MAILB");
        break;
    case 254:
        strcpy(value, "MAILA");
        break;
    case 255:
        strcpy(value, "*");
        break;
    default:
        id = type + '0';
        strcpy(value, strcat("Type", &id));
        break;
    }
}
