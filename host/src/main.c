#define _XOPEN_SOURCE 700 // necessary to not get errors with the hints struct.
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server.h"

int main()
{
    int success;
    server_t server;

    if (0 != setup_server(&server))
    {
        fprintf(stderr, "Server setup failed!\n");
        return -1;
    }

    success = accept_connections(&server);
    if (!success)
    {
        printf("Error accepting connections\n");
    }

    return 0;
}