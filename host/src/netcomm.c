#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Function to send all data, returns 1 on success, 0 on failure
int send_all(int sock, const char *data, size_t data_len)
{
    size_t total_sent_bits = 0;
    while (total_sent_bits < data_len)
    {
        ssize_t sent_bits = send(sock, data + total_sent_bits, data_len - total_sent_bits, 0);
        if (sent_bits == -1)
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
    char buffer[BUFFER_SIZE];
    size_t total_len = 0;
    char *message = NULL;

    while (1)
    {
        ssize_t received = recv(sock, buffer, sizeof(buffer), 0);
        if (received <= 0)
        {
            free(message);
            return NULL;
        }
        // interpreting byte by byte use realloc to adjust the size
        char *new_msg = realloc(message, total_len + received + 1);
        if (!new_msg)
        {
            free(message);
            return NULL;
        }
        message = new_msg;

        // dest, src, size (Storing msg from buffer to variable)
        memcpy(message + total_len, buffer, received);
        total_len += received;

        for (ssize_t i = 0; i < received; ++i)
        {
            if (buffer[i] == '\0')
            {
                message[total_len] = '\0';
                return message;
            }
        }
    }
}