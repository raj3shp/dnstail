#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "dns_protocol.h"

void parse_dns_query(struct dns_query *dns_qr, int query_size, struct dns_packet *pkt)
{
    u_int16_t i, j, type;
    char *fqdn;
    char *question;

    question = pkt->data;

    // max query size 253 characters
    fqdn = calloc(MAX_QUERY_SIZE, sizeof(char));

    j = 0;
    while (j <= query_size)
    {
        int length = question[j];
        // length 0 indicates end of query
        if (length == 0)
            break;
        for (i = 1; i <= length; i++)
        {
            fqdn[(i - 1) + j] = question[i + j];
        }
        strcat(fqdn, ".");
        j += (length + 1);
    }   
    int query_len = strlen(fqdn);
    type = question[query_len+2];
    strcpy(dns_qr->qname, fqdn);
    dns_qr->type = type;

    // don't forget to cleanup
    free(fqdn);
}

void get_query_type_value(u_int16_t type, char *value)
{
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
    default:
        strcpy(value, "INVALID");
        break;
    }
}