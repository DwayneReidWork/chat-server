#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_MSG_SIZE 1024

// Function to send all data, returns 1 on success, 0 on failure
int send_all(int sock, const char *data, size_t data_len)
{
    size_t total_sent_bits = 0;
    while (total_sent_bits < data_len)
    {
        ssize_t sent_bits = send(sock, data + total_sent_bits, data_len - total_sent_bits, 0);
        if (-1 == sent_bits)
        {
            return 0; // Return false if send fails
        }
        total_sent_bits += sent_bits;
    }
    return 1; // Success
}

// Receive from socket until hitting a null char.
char *recv_until_null(int sock)
{
    char buffer[MAX_MSG_SIZE];
    size_t count = 0;

    while (count < MAX_MSG_SIZE - 1)
    {
        char byte;
        ssize_t received = recv(sock, &byte, 1, 0);
        if (received <= 0)
        {
            return NULL;
        }

        buffer[count++] = byte;
        if (byte == '\0')
        {
            break;
        }
    }

    char *message = malloc(count);
    if (!message)
    {
        return NULL;
    }

    strncpy(message, buffer, count);
    return message;
}
