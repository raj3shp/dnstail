#ifndef __PROXY_H__
#define __PROXY_H__

void proxy_dns_reuqest(char *req_buf, int req_size, struct sockaddr_in from, int sd);

#endif