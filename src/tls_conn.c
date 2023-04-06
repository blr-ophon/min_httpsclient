#include "tls_conn.h"

//Create ssl struct and initializes TLS connection to the url hostname through specified socket
SSL *TLS_init(SSL_CTX *ctx, struct parsed_url *url, int sockfd){ 
    //Create SSL struct from context
    SSL *ssl = SSL_new(ctx);        
    if(!ssl){
        return NULL;
    }

    //set domain for the server, so it know which certificate it sends in the
    //case of having more than one site
    if(!SSL_set_tlsext_host_name(ssl, url->hostname)){
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    //set socket that will be used for TLS connection
    SSL_set_fd(ssl, sockfd);

    //Connect to server
    if(SSL_connect(ssl) < 0){
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    printf("TLS connection sucessful\n");
    printf("Using: %s\n", SSL_get_cipher(ssl));
    show_cert_info(ssl);
    return ssl;
}

void show_cert_info(SSL *ssl){
    //get certificate
    X509 *cert = SSL_get_peer_certificate(ssl);
    if(!cert){
        return;        
    }

    char *rv;
    //Subject from certificate
    if((rv = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)) != NULL){
        printf("Subject: %s\n", rv);
        OPENSSL_free(rv);
    }

    //Issuer from certificate
    if((rv = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)) != NULL){
        printf("Issuer: %s\n", rv);
        OPENSSL_free(rv);
    }
    X509_free(cert);
}
