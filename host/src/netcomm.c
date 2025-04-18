#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// FOR HANDLING PARTIAL SENDS AND RECIEVES

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

char *recv_expected(int sock, size_t expected_len)
{
    char *recvd_bytes = (char *)malloc(expected_len);
    if (recvd_bytes == NULL)
    {
        return NULL; // Memory allocation failed
    }

    size_t recvd_len = 0;
    while (recvd_len < expected_len)
    {
        ssize_t received = recv(sock, recvd_bytes + recvd_len, expected_len - recvd_len, 0);
        if (received == -1)
        {
            free(recvd_bytes);
            return NULL; // Return NULL if recv fails
        }
        recvd_len += received;
    }
    return recvd_bytes;
}