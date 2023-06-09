#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "hhl.h"
#include "tls_conn.h"

SSL *TLS_init(SSL_CTX *ctx, struct parsed_url *url, int sockfd);

void show_cert_info(SSL *ssl);

int socket_init(struct parsed_url *url);

int try_connection(struct addrinfo *const addresses);

void print_addr(struct addrinfo *addr);

#endif
