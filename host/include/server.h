#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h> // For struct sockaddr_in
#include <sys/types.h>  // For socklen_t

#define _XOPEN_SOURCE 700 // necessary to not get errors with the hints
#define PORT 4444
#define PORT_STR "4444"
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10

typedef struct
{
    int socket_fd;
    struct sockaddr_in address;
    socklen_t addrlen;
} server_t;

// from server.c
int accept_connections(server_t *server);
void broadcast_to_all_clients(const char *msg, int sender_socket);
void *handle_client(void *arg);
int setup_server(server_t *server);

// from netcomm.c
int send_all(int sock, const char *data, size_t data_len);
char *recv_until_null(int sock);
void add_client(int client_socket);
void remove_client(int client_socket);

#endif