#ifndef TINYWEB_H
#define TINYWEB_H

#include <netinet/in.h>

/* Buffer size for http response */
#define RESP_BUF_SIZE 8192

int web_open(int port);

char *web_recv(int fd, struct sockaddr_in *clientaddr);

void web_send(int out_fd, char *buffer);

int web_eventmux(char *buf);

#endif
