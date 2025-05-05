#define _XOPEN_SOURCE 700 // necessary to not get errors with the hints struct.
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"

server_t server;
_Atomic bool should_quit = false;

/*Signal Handler for ctrl+c*/
void signal_handler(int num)
{
    should_quit = true;
    write(STDOUT_FILENO, "\nKill signal received.\nShould Quit set to true\n", 47);
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

    return success;
}