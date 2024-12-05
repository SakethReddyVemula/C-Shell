#ifndef __IMAN_H_
#define __IMAN_H_

typedef struct {
    int sockfd;
    struct addrinfo *server_info;
} Connection;

Connection create_connection(char *host, char *port);
char* send_request(Connection *conn, char *token);
void print_man_page(char *response);
void iMan(char *token);

#endif