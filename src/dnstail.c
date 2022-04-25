#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "dns_protocol.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
    int port;
    char *host;
    host = calloc(15, sizeof(char));

    if (argc != 3)
    {
        printf("usage: dnstail <ip> <port>\n");
        exit(1);
    }
    strcpy(host, argv[1]);
    port = atoi(argv[2]);
    dns_start(port, host);
    free(host);
}
