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
#include <sys/wait.h>

#include "server.h"
#include "dns_protocol.h"
#include "proxy.h"

void dns_start(int port, char *host)
{
    int sd;
    struct sockaddr_in sin;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("dnstail: error opening socket");
        exit(1);
    }
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host);
    sin.sin_port = htons(port);
    if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)))
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
    char buf[PACKET_SIZE + 4];

    char *req_buf;
    struct sockaddr_in from;
    socklen_t from_addr_size;
    from_addr_size = sizeof(from);

    for (;;)
    {
        req_buf = malloc(PACKET_SIZE + 4);
        recv_size = recvfrom(sd, buf, PACKET_SIZE + 4, 0, (struct sockaddr *)&from, &from_addr_size);
        memcpy(req_buf, buf, recv_size);
        if (fork() == 0)
        {
            proxy_dns_reuqest(req_buf, recv_size, from, sd);
            exit(0);
        }
        signal(SIGCHLD,SIG_IGN);
        free(req_buf);
        continue;
    }
}
