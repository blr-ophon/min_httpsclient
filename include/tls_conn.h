#ifndef TLS_CONN_H
#define TLS_CONN_H

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

SSL *TLS_init(SSL_CTX *ctx, struct parsed_url *url, int sockfd);

void show_cert_info(SSL *ssl);

void verification_results(SSL *ssl);

void get_http_ver(struct parsed_url *url);

#endif
