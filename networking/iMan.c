#include "../utils/headers.h"

// typedef struct {
//     int sockfd;
//     struct addrinfo *server_info;
// } Connection;

Connection create_connection(char *host, char *port){
    Connection conn = {-1, NULL};
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    };

    int status = getaddrinfo(host, port, &hints, &conn.server_info);
    if(status != 0){
        fprintf(stderr, BRED "ERROR: Failed to get address info\n" CRESET);
        return conn;
    }

    for(struct addrinfo *p = conn.server_info; p != NULL; p = p->ai_next){
        conn.sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(conn.sockfd == -1){
            continue;
        }
        if(connect(conn.sockfd, p->ai_addr, p->ai_addrlen) != -1){
            break;
        }

        close(conn.sockfd);
        conn.sockfd = -1;
    }

    if(conn.sockfd == -1){
        fprintf(stderr, BRED "ERROR: Failed to connect to server\n" CRESET);
        freeaddrinfo(conn.server_info);
        conn.server_info = NULL;
    }

    return conn;
}

char* send_request(Connection *conn, char *token){
    char request[MAX_BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n", token, MAN_HOST);
    
    if(send(conn->sockfd, request, strlen(request), 0) == -1){
        fprintf(stderr, BRED "ERROR: Failed to send request\n" CRESET);
        return NULL;
    }

    char *response = malloc(MAX_BUFFER_SIZE);
    if(response == NULL){
        fprintf(stderr, BRED "ERROR: Failed to allocate memory for response\n" CRESET);
        return NULL;
    }

    int total_received = 0;
    int bytes_received;
    while((bytes_received = recv(conn->sockfd, response + total_received, MAX_BUFFER_SIZE - total_received - 1, 0)) > 0){
        total_received += bytes_received;
        if(total_received >= MAX_BUFFER_SIZE - 1){
            break;
        }
    }

    if(bytes_received == -1){
        fprintf(stderr, BRED "ERROR: Failed to receive response\n" CRESET);
        free(response);
        return NULL;
    }

    response[total_received] = '\0';
    return response;
}

void print_man_page(char *response) {
    // first, check if we have a valid HTTP response
    if (strncmp(response, "HTTP/1.", 7) != 0) {
        fprintf(stderr, BRED "ERROR: Invalid HTTP response\n" CRESET);
        return;
    }

    // find the end of the headers
    char *body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        // If we can't find "\r\n\r\n", try looking for "\n\n"
        body_start = strstr(response, "\n\n");
        if (body_start == NULL) {
            fprintf(stderr, BRED "ERROR: Cannot find start of response body\n" CRESET);
            return;
        }
        body_start += 2; // skip the \n\n
    } else {
        body_start += 4; // skip the \r\n\r\n
    }

    // frim leading newlines
    while (*body_start == '\n' || *body_start == '\r') {
        body_start++;
    }

    // find the start of the actual content (after any potential headers in the body)
    char *content_start = body_start;
    char *line_end;
    while ((line_end = strchr(content_start, '\n')) != NULL) {
        if (line_end == content_start || line_end[-1] == '\r') {
            // empty line found, content starts after this
            content_start = line_end + 1;
            break;
        }
        content_start = line_end + 1;
    }

    // find and remove the footer if present
    char *footer_start = strstr(content_start, "Copyright");
    if (footer_start) {
        *footer_start = '\0';
    }

    // trim trailing whitespace
    char *end = content_start + strlen(content_start) - 1;
    while (end > content_start && (*end == ' ' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    if (strlen(content_start) == 0) {
        fprintf(stderr, BRED "ERROR: No content found\n" CRESET);
    } else {
        printf("%s\n", content_start);
    }
}

void iMan(char *token){
    Connection conn = create_connection(MAN_HOST, HTTP_PORT);
    if(conn.sockfd == -1){
        return;
    }

    char *response = send_request(&conn, token);
    if(response){
        // printf("%s\n", response);
        print_man_page(response);
        free(response);
    }

    close(conn.sockfd);
    freeaddrinfo(conn.server_info);
}