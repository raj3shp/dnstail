#ifndef __SERVER_H__
#define __SERVER_H__

void dns_start(int port, char *host);
void process_request(int sd);

#endif