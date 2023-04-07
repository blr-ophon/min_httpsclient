#include "tls_conn.h"

//TODO: Must include a list of trusted root certificates

//Create ssl struct and initializes TLS connection to the url hostname through specified socket
SSL *TLS_init(SSL_CTX *ctx, struct parsed_url *url, int sockfd){ 
    //Verify certificates using trusted list
    if(!SSL_CTX_load_verify_locations(ctx, "./certs/cacert.pem", 0)){
        ERR_print_errors_fp(stderr);
    }

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

    //Print results from certificate validation
    verification_results(ssl);

}

void verification_results(SSL *ssl){
    //Show validation results
    long vp = SSL_get_verify_result(ssl);
    if(vp == X509_V_OK){
        printf("Certificates validation OK\n");
    }else{
        printf("Certificates validation FAILED\n");
    }
}

void get_http_ver(struct parsed_url *url){
    free(url->protocol);
    url->protocol = calloc(5, 1);
    strncpy(url->protocol, "http", 5);
    free(url->port);
    url->port = calloc(3, 1);
    strncpy(url->protocol, "80", 3);
}


