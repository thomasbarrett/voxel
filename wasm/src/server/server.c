#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <assert.h>
#include <server/message.h>

#define PORT 3000

float positions[5][3] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
};

block_updates_t block_updates;

typedef enum http_method {
    GET, PUT, POST, DELETE, OPTIONS
} http_method_t;

typedef struct http_request {
    http_method_t method;
    size_t content_length;
    char *body;
    char buffer[2 << 14];
} http_request_t;

void enable_keepalive(int sock) {
    int yes = 1;
    assert(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

    int interval = 1;
    assert(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

    int maxpkt = 10;
    assert(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);

    int reuse_addr = 1;
    assert(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse_addr,sizeof(int)) != -1);
}

size_t http_request_parse_method(http_request_t *self, char *s) {
    if (memcmp(s, "GET", 3) == 0) {
        self->method = GET;
        return 3;
    } else if (memcmp(s, "PUT", 3) == 0) {
        self->method = PUT;
        return 3;
    } else if (memcmp(s, "POST", 4) == 0) {
        self->method = POST;
        return 4;
    } else if (memcmp(s, "DELETE", 6) == 0) {
        self->method = DELETE;
        return 6;
    } else if (memcmp(s, "OPTIONS", 7) == 0) {
        self->method = OPTIONS;
        return 7;
    } else {
        return 0;
    }
}

void nextline(char *s, size_t *pos) {
    while (s[*pos] != '\n') *pos += 1;
    *pos += 1;
}

void http_request_parse(http_request_t *self) {
    size_t pos = 0;
    http_request_parse_method(self, (char*) &self->buffer);
    nextline((char*) &self->buffer, &pos);
    if (self->method == POST) {
        while(memcmp(&self->buffer[pos], "\r\n", 2) != 0) {
            nextline((char*) &self->buffer, &pos);
        }
        pos += 2;
        char *body = &self->buffer[pos];

        message_t *message_type = (message_t*) body;
        if (*message_type == POSITION_UPDATE) {
            position_update_t *u = (position_update_t*) body;
            printf("POSITION UPDATE: pid: %d, x: %f, y: %f, z: %f\n", u->pid, u->x, u->y, u->z);
            if (u->pid >= 0 && u->pid < 5) {
                positions[u->pid][0] = u->x;
                positions[u->pid][1] = u->y;
                positions[u->pid][2] = u->z;
            } else {
                printf("error: invalid pid received");
            }
        } else if (*message_type == BLOCK_UPDATE) {
            block_update_t *u = (block_update_t*) body;
            printf("BLOCK UPDATE: pid: %d, x: %d, y: %d, z: %d, block: %d\n", u->pid, u->x, u->y, u->z, u->block);
            memcpy(&block_updates.updates[block_updates.length], u, sizeof(block_update_t));
            block_updates.length++;
        } else {
            printf("%s", body);
        }
    }
}

typedef struct http_response {
    char buffer[2 << 14];
    size_t length;
} http_response_t;

void http_response_init(http_response_t *self) {
    memset(&self->buffer, 0, 2 << 14);
    self->length = 0;
}

void http_response_write(http_response_t *self, const char *header) {
    memcpy((char *) &self->buffer + self->length, header, strlen(header));
    self->length += strlen(header);
}

void http_response_writen(http_response_t *self, void *data, int n) {
    memcpy((char *) &self->buffer + self->length, data, n);
    self->length += n;
}

int main(int argc, char const *argv[])
{
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
        
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address))<0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    printf("listening on port %d\n", PORT);
    while(1) {
        
        int socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if (socket < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        enable_keepalive(socket);

        http_request_t request = {0};
        long read_result = read(socket, (char *) &request.buffer, 2 << 14);    
        http_request_parse(&request);

        if (request.method != OPTIONS) {
            http_response_t response;
            http_response_init(&response);
            http_response_write(&response, "HTTP/1.1 200 OK\r\n");
            http_response_write(&response, "Connection: keep-alive\r\n");
            http_response_write(&response, "Access-Control-Allow-Origin: *\r\n");
            http_response_write(&response, "Content-Type: binary\r\n\r\n");
            positions_update_t u;
            u.message = POSITIONS_UPDATE;
            memcpy(&u.positions, &positions, sizeof(positions));
            bulk_update_t update;
            memcpy(&update.position_update, &u, sizeof(positions_update_t));
            memcpy(&update.block_updates, &block_updates, sizeof(block_updates_t));
            http_response_writen(&response, &update, sizeof(bulk_update_t));
            write(socket, &response.buffer, response.length);
            close(socket);
        } else {
            http_response_t response;
            http_response_init(&response);
            http_response_write(&response, "HTTP/1.1 204 No Content\r\n");
            http_response_write(&response, "Connection: keep-alive\r\n");
            http_response_write(&response, "Access-Control-Allow-Origin: *\r\n");
            http_response_write(&response, "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n");
            http_response_write(&response, "Access-Control-Max-Age: 86400\r\n\r\n");
            write(socket, &response.buffer, response.length);
            close(socket);
        }
        
    }
    return 0;
}