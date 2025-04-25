#define _XOPEN_SOURCE 700 // necessary to not get errors with the hints struct.
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"

server_t server;
volatile sig_atomic_t kill_child_threads = 0;

void signal_handler(int num)
{
    kill_child_threads = 1;
    // remove_all_clients();
    // close(server.socket_fd);
    // exit(EXIT_SUCCESS);
}

int main()
{
    int success;

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    if (0 != setup_server(&server))
    {
        fprintf(stderr, "Server setup failed!\n");
        return -1;
    }

    success = accept_connections(&server);
    if (0 != success)
    {
        printf("Error accepting connections\n");
    }

    return 0;
}