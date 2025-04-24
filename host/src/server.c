// the definition of struct addrinfo is part of the POSIX standards. However, on some systems, the default standard libraries may not provide the necessary definitions or may limit certain features unless explicitly requested. By defining _XOPEN_SOURCE with a value like 700, you are telling the compiler to enable the POSIX-2008 features

// #define _XOPEN_SOURCE 700 // necessary to not get errors with the hints struct.

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"
#include "netcomm.c"

// create a struct with clients and thread
int clients[MAX_CONNECTIONS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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

    // Setup server address information
    // server->address = *((struct sockaddr_in *)p->ai_addr);
    //  Use the first valid address found
    // server->addrlen = p->ai_addrlen;
    memcpy(&server->address, p->ai_addr, p->ai_addrlen);
    // Free the addrinfo struct

    freeaddrinfo(result);

    if (-1 == server->socket_fd)
    {
        fprintf(stderr, "Failed to bind to any address\n");
        return -1;
    }

    if (listen(server->socket_fd, MAX_CONNECTIONS) < 0)
    {
        perror("Listen failed");
        close(server->socket_fd);
        return -1;
    }

    // printf("Server listening on port 4444...\n");
    printf("Server listening on port %d...\n", PORT);
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    printf("Client connected. Socket #%d\n", client_socket);
    fflush(stdout);

    // Continuously receive data from the client
    while (1)
    {
        // recv_expected returns ptr to null terminated string
        // using buffer doesnt work because its too big so its never reaches.
        char *msg = recv_until_null(client_socket);
        if (NULL != msg)
        {
            printf("Socket %d sent: %s\n", client_socket, msg);
            broadcast_to_all_clients(msg, client_socket);

            free(msg);
        }
        else
        {
            // Client disconnected or recv failed
            printf("Client on socket %d disconnected.\n", client_socket);
            close(client_socket);
            remove_client(client_socket);
            break;
        }
    }
    return 0;
}

int accept_connections(server_t *server)
{
    struct sockaddr_storage client_addr;

    socklen_t client_addr_len = sizeof(client_addr);

    int new_connection;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        // set all spots in client array to -1 for available
        clients[i] = -1;
    }

    while (1)
    {
        new_connection = accept(server->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (new_connection < 0)
        {
            perror("accept failed");
            continue;
        }

        add_client(new_connection);
        printf("\nConnection received!\n");
        fflush(stdout);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, &new_connection) != 0)
        {
            perror("pthread_create failed");
            close(new_connection);
        }
        else
        {
            pthread_detach(thread_id); // Automatically release resources when thread exits
        }
    }

    return 0;
}

void broadcast_to_all_clients(const char *msg, int sender_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; ++i)
    {
        // we have clients and not broacasting to self.
        if (clients[i] != -1 && clients[i] != sender_socket)
        {
            char reply_buffer[100];
            int result = snprintf(reply_buffer, sizeof(reply_buffer), "User %d: %s", sender_socket, msg);

            if (result >= 0 && result < sizeof(reply_buffer))
            {
                send_all(clients[i], reply_buffer, strlen(reply_buffer) + 1);
            }
            else
            {
                printf("Error: String too long or encoding error\n");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Add client to the global clients[] array
void add_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (clients[i] == -1)
        {
            // if slot is -1 its open to be used
            clients[i] = client_socket;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_socket)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (clients[i] == client_socket)
        {
            // set client slot back to -1 ("remove")
            clients[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_all_clients()
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (clients[i] != -1)
        {
            close(clients[i]);
            clients[i] = -1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}