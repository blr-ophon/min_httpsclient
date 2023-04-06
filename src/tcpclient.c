#include "tcpclient.h"
//TODO: Remove code from main

#define HTTPS_PORT "443"

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: minhttp <url>\n");
        return -1;
    }


    //Initialize SSL and create context
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if(!ctx){
        fprintf(stderr, "Failed creating SSL context\n");
        return -1;
    }


    //Parse URL
    struct parsed_url url;
    parse_url(&url, argv[1]);
    
    //Initialize TCP connection
    int sockfd = socket_init(&url);
    if(sockfd < 0){
        fprintf(stderr,"Hostname can't be reached. Connection failed\n");
        exit(EXIT_FAILURE);
    }

    //Initialize TLS connection
    SSL *ssl = TLS_init(ctx, &url, sockfd);
    if(!ssl){ 
        fprintf(stderr, "TLS connection failed\n"); 
        exit(EXIT_FAILURE);
    }
    SSL_CTX_free(ctx);

    char send_msg_buf[8182] = {0};
    char recv_msg_buf[8192] = {0};      //to receive each individual packet received

    char *full_recv_msg = NULL;         //to store complete message from arrived packets
    int received_count = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;

    //Prompt for method and send HTTP query
    printf("Enter Method: \n");
    char method[10];
    fgets(method, sizeof(method), stdin);
    method[strlen(method)-1] = '\0';    //remove \n
    httpmsg_setHeader(&url, method, send_msg_buf);
    int bytes_sent = SSL_write(ssl, send_msg_buf, strlen(send_msg_buf));
    if(bytes_sent == -1){
        perror("recv");
    }

    //Receive messages until server closes connection
    for(;;){
        int recv_bytes = SSL_read(ssl, recv_msg_buf, sizeof(recv_msg_buf));
        recv_msg_buf[recv_bytes] = '\0';
        if(recv_bytes < 1){
            printf("\n---Server connection closed\n");
            break;
        }else{
            int temp = received_count;
            received_count += recv_bytes;
            full_recv_msg = realloc(full_recv_msg, received_count);
            memcpy(&full_recv_msg[temp], recv_msg_buf, recv_bytes);
        }
    }


    if(full_recv_msg){
        full_recv_msg[received_count] = '\0';
    }
    printf("\n---Received response:\n\n%s\n", full_recv_msg);

    httpmsg_handleResponse(full_recv_msg, &url);

    free(full_recv_msg);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
}

//Returns a socket connected to the specified domain or -1 on failure
int socket_init(struct parsed_url *url){
    int sockfd = -1;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addresses = NULL;
    int rv = getaddrinfo(url->hostname, HTTPS_PORT , &hints, &addresses);
    if(rv < 0){
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    sockfd = try_connection(addresses); //returns -1 on failure
    freeaddrinfo(addresses);
    return sockfd;
}

//Traverses an addrinfo linked list, stoping at the first one able to connect
//returns socket file descriptor or -1 if it failed
int try_connection(struct addrinfo *const addresses){
    int sockfd = -1;
    struct addrinfo *p;
    for(p = addresses; p != NULL; p = p->ai_next){
        printf("Trying:\n");
        print_addr(p);

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd < 0){
            sockfd = -1;
            perror("socket()");
            continue;
        }
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) < 0){
            sockfd = -1;
            perror("connect()");
            continue;
        }
        break;
    }
    if(sockfd < 0){
        return -1;
    }
    
    printf("---Connection established!\n");
    return sockfd;
}


void print_addr(struct addrinfo *addr){
        char host_buf[100]; 
        char serv_buf[100];
        int rv = getnameinfo(addr->ai_addr, addr->ai_addrlen, host_buf, sizeof(host_buf),
                serv_buf, sizeof(serv_buf), NI_NUMERICHOST | NI_NUMERICSERV);
        if(rv < 0){
            printf("getnameinfo() failed\n");
        }
        printf("%s\n%s\n", host_buf, serv_buf);
}

