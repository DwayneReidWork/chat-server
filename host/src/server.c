// the definition of struct addrinfo is part of the POSIX standards. However, on some systems, the default standard libraries may not provide the necessary definitions or may limit certain features unless explicitly requested. By defining _XOPEN_SOURCE with a value like 700, you are telling the compiler to enable the POSIX-2008 features

// #define _XOPEN_SOURCE 700 // necessary to not get errors with the hints struct.

#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server.h"
#include "netcomm.c"

int clients[MAX_CONNECTIONS];

int setup_server(server_t *server)
{

    int status = 0;
    // for reuse (optVal) 0 lets other servers to bind and listen
    // 1 allows other servers to bind and listen to the port and to connect
    int reuse = 1;
    struct addrinfo hints = {0};
    struct addrinfo *result, *p;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // any interface + only port strings, no names
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    // any protocol
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // Getaddrinfo uses string interpretation of port htons uses int.
    status = getaddrinfo(NULL, PORT_STR, &hints, &result);
    if (0 != status)
    {
        // gai_strerror is specific to get address info makes errors readable.
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
        return -1;
    }
    // set p = to the vertical iteration we're on then ai_next iterates horizontally
    for (p = result; p != NULL; p = p->ai_next)
    {
        server->socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server->socket_fd == -1)
        {
            perror("Socket creation failed");
            continue; // Try the next result if socket creation fails
        }

        // Set socket options
        if (-1 == setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
        {
            perror("setsockopt SO_REUSEADDR failed");
            close(server->socket_fd);
            continue;
        }

        if (-1 == setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)))
        {
            perror("setsockopt SO_REUSEPORT failed");
            close(server->socket_fd);
            continue;
        }

        // Bind the socket to the resolved address
        if (-1 == bind(server->socket_fd, p->ai_addr, p->ai_addrlen))
        {
            perror("Bind failed");
            close(server->socket_fd);
            continue;
        }

        break;
    }

    // Free the addrinfo struct
    freeaddrinfo(result);

    if (-1 == server->socket_fd)
    {
        fprintf(stderr, "Failed to bind to any address\n");
        return -1;
    }

    // Setup server address information
    server->address = *((struct sockaddr_in *)p->ai_addr);
    // Use the first valid address found
    server->addrlen = p->ai_addrlen;

    if (listen(server->socket_fd, MAX_CONNECTIONS) < 0)
    {
        perror("Listen failed");
        close(server->socket_fd);
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);

    printf("Client connected. Socket #%d\n", client_socket);
    fflush(stdout);

    // Continuously receive data from the client
    while (1)
    {
        // recv_expected returns ptr to null terminated string
        // using buffer doesnt work because its too big so its never reaches.
        char *msg = recv_until_null(client_socket);
        if (msg)
        {
            printf("Socket %d sent: %s\n", client_socket, msg);
            broadcast_to_all_clients(msg, client_socket);

            free(msg);
        }
    }
}

int accept_connections(server_t *server)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        clients[i] = -1;
    }

    while (1)
    {
        int *new_connection = malloc(sizeof(int));
        if (!new_connection)
        {
            perror("malloc failed");
            continue;
        }

        *new_connection = accept(server->socket_fd, (struct sockaddr *)&server->address, &server->addrlen);
        if (*new_connection < 0)
        {
            perror("accept failed");
            free(new_connection);
            continue;
        }

        add_client(*new_connection);
        printf("\nConnection received!\n");
        fflush(stdout);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, new_connection) != 0)
        {
            perror("pthread_create failed");
            close(*new_connection);
            free(new_connection);
        }
        else
        {
            pthread_detach(thread_id); // Automatically release resources when thread exits
        }
    }

    return 0;
}

// WIP
void broadcast_to_all_clients(const char *msg, int sender_socket)
{
    for (int i = 0; i < MAX_CONNECTIONS; ++i)
    {
        // we have clients and not broacasting to self.
        if (clients[i] != -1 && clients[i] != sender_socket)
        {
            send_all(clients[i], msg, strlen(msg) + 1);
        }
    }
}

// Add client to the global clients[] array
void add_client(int client_socket)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (clients[i] == -1)
        {
            clients[i] = client_socket;
            break;
        }
    }
}

void remove_client(int client_socket)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (clients[i] == client_socket)
        {
            clients[i] = -1;
            break;
        }
    }
}
