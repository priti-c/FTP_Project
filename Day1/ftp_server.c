#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 2121
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind to port 2121
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    memset(address.sin_zero, 0, sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("FTP Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        // Send greeting
        send(new_socket, "220 FTP Server Ready\r\n", strlen("220 FTP Server Ready\r\n"), 0);

        int logged_in = 0; // Track login status

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) break;

            printf("Received: %s", buffer);

            if (strncmp(buffer, "USER", 4) == 0) {
                send(new_socket, "230 Login successful\r\n", strlen("230 Login successful\r\n"), 0);
                logged_in = 1;
            } else if (strncmp(buffer, "PASS", 4) == 0) {
                send(new_socket, "230 Login successful\r\n", strlen("230 Login successful\r\n"), 0);
                logged_in = 1;
            } else if (strncmp(buffer, "QUIT", 4) == 0) {
                send(new_socket, "221 Goodbye\r\n", strlen("221 Goodbye\r\n"), 0);
                break;
            } else {
                // Unknown command
                send(new_socket, "502 Command not implemented\r\n", strlen("502 Command not implemented\r\n"), 0);
            }
        }

        close(new_socket);
        printf("Client disconnected.\n");
    }

    close(server_fd);
    return 0;
}