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
    size_t recvd_len = 0;
    char *buffer = malloc(expected_len + 1);
    if (!buffer)
    {
        perror("malloc failed");
        return NULL;
    }

    while (recvd_len < expected_len)
    {
        ssize_t received = recv(sock, buffer + recvd_len, expected_len - recvd_len, 0);
        if (received <= 0)
        {
            free(buffer);
            return NULL;
        }

        recvd_len += received;
    }

    buffer[expected_len] = '\0';
    return buffer;
}

// NEEDS TO GO SOMEWHERE ELSE!!!!!!
//      // Find null byte
//      size_t raw_data_before_null_byte = 0;
//      while (raw_data_before_null_byte < recvd_len && recvd_bytes[raw_data_before_null_byte] != '\0')
//      {
//          raw_data_before_null_byte++;
//      }

//     // memmory allocate length of data before null byte +1 for null
//     char *null_terminted_data = malloc(raw_data_before_null_byte + 1);
//     if (null_terminted_data == NULL)
//     {
//         free(recvd_bytes);
//         return NULL;
//     }

//     // Copy the data
//     memcpy(null_terminted_data, recvd_bytes, raw_data_before_null_byte);
//     null_terminted_data[raw_data_before_null_byte] = '\0';

//     printf("Null Terminated Data: %s\n", null_terminted_data);
//     fflush(stdout);
//     free(recvd_bytes);

//     return null_terminted_data;
// }