#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080           // Port number the server will use
#define MAX_CLIENTS 4       // Maximum number of clients the server can handle

int main()
{
    int server_fd, client_fds[MAX_CLIENTS] = {0}; // Initialize client socket array to 0
    int max_sd, activity, new_socket, sd, i;
    struct sockaddr_in server_address;
    socklen_t addrlen = sizeof(server_address);
    char buffer[1024] = {0}; // Buffer for client messages
    fd_set readfds;

    // Create the server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the server socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for client connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        // Clear and set up the file descriptor set
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds); // Add server socket to the set
        max_sd = server_fd;

        // Add all active client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_fds[i];
            if (sd > 0) // If the client socket is active
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) // Update max descriptor
            {
                max_sd = sd;
            }
        }

        // Wait for activity on any socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("Select error");
        }

        // Check if there's a new client connection
        if (FD_ISSET(server_fd, &readfds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&server_address, &addrlen)) < 0)
            {
                perror("Accept failed");
                continue;
            }

            printf("New client connected: socket %d\n", new_socket);

            // Add the new socket to the array of clients
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fds[i] == 0)
                {
                    client_fds[i] = new_socket; // Add new socket
                    printf("Adding client to list at index %d\n", i);
                    break;
                }
            }

            // Send a welcome message to the new client
            send(new_socket, "Welcome to the server!\n", 23, 0);
        }

        // Check for messages from clients
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_fds[i];

            if (FD_ISSET(sd, &readfds))
            {
                int valread = read(sd, buffer, sizeof(buffer) - 1);

                if (valread == 0)
                {
                    // Client disconnected
                    printf("Client disconnected: socket %d\n", sd);
                    close(sd);
                    client_fds[i] = 0;
                }
                else
                {
                    buffer[valread] = '\0'; // Null-terminate the message
                    printf("Client %d says: %s\n", sd, buffer);

                    // Broadcast message to all other clients
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (client_fds[j] != 0 && client_fds[j] != sd)
                        {
                            send(client_fds[j], buffer, valread, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
