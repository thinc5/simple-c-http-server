#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"

volatile sig_atomic_t running = true;

/**
 * A simple function to show the server address, given a refernce to a
 * sockaddr_in struct.
 */
void show_server_address(struct sockaddr_in *server_address)
{
    char host_buffer[INET6_ADDRSTRLEN];
    char port_buffer[NI_MAXSERV];
    socklen_t addr_len = sizeof(*server_address);
    int err = getnameinfo((struct sockaddr *)server_address, addr_len,
                          host_buffer, sizeof(host_buffer), port_buffer,
                          sizeof(port_buffer), NI_NUMERICHOST);
    printf("Listening on %s:%s\n", host_buffer, port_buffer);
}

/**
 * Display the connected peer's ip address.
 * Not much use when the connection is proxied.
 */
void display_peer_ip(struct sockaddr *client_address)
{
    char buff[100];
    socklen_t size = 100;
    inet_ntop(AF_INET, client_address, buff, size);
    printf("Connection from: %s\n", buff);
}

/**
 * Check if the incoming data is valid http.
 */
bool is_valid_http(char *message)
{
    // Read along the message line by line.
    char *slice = strtok(message, "\r\n");
    while (slice != NULL)
    {
        printf("Slice:\n", slice);
        slice = strtok(NULL, "\r\n");
    }
}

/**
 * Continuously read from a provided socket until we read
 * 0 bytes from it. Buffer is copied into an allocated string.
 */
void read_peer_data(int s)
{
    // Use select to prevent recv from blocking until there is something to read.
    sigset_t mask;
    sigset_t other_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(1, &mask, &other_mask);
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(s, &fds);
    struct timespec timeout = {.tv_nsec = 300};
    pselect(s + 1, &fds, NULL, NULL, &timeout, NULL);
    fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK);
    // Actually read.
    char buf[4096];
    ssize_t bytes_read;
    do
    {
        if (!running)
        {
            return;
        }
        bytes_read = recv(s, buf, sizeof(buf), 0);
    } while (bytes_read > 0);
    is_valid_http(buf);
}

/**
 * Signal handler, catches sigint and terminates the server's
 * socket connection.
 */
void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        printf("received SIGINT\n");
        running = false;
    }
}

/**
 * Single threaded application, which handles incoming TCP traffic
 * via ipv4 looking for whitelisted HTTP POST requests, then performs
 * preditermined tasks.
 *
 * This program attempts to follow the suckless philosophy and configuration
 * options can be found in config.h.
 */
int main()
{
    // Create a socket, using ipv4 and tcp.
    int yes = 1;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // Specify the address to bind to.
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(inet_addr(LISTEN_ADDR));
    // Bind the socket to the address.
    bind(server_socket, (struct sockaddr *)&address, sizeof(address));
    // Attempt to listen on bound socket.
    listen(server_socket, 20);
    // Show the address we are listening on.
    show_server_address(&address);

    /**
     * Register signal handler, we can do this now since we are prepared
     * to exit gracefully.
     */
    struct sigaction sig_int;
    memset(&sig_int, 0, sizeof(sig_int));
    sig_int.sa_handler = signal_handler;
    sigaction(SIGINT, &sig_int, 0);

    // Handle all incoming connections.
    int client_socket;
    while (running)
    {
        // Accept incoming connection.
        struct sockaddr_in client_address;
        socklen_t sock_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &sock_size);

        // Read incoming connection ip address.
        display_peer_ip((struct sockaddr *)&client_address.sin_addr);

        // Read data if any is avaliable.
        read_peer_data(client_socket);
        printf("Data read, replying...\n");
        // Write back a 200 if we are happy with the sender.
        printf("Writing a 200 back to the client.\n%s\n", MINIMAL_HTTP_RESPONSE_HEADDER);
        send(client_socket, MINIMAL_HTTP_RESPONSE_HEADDER, sizeof(MINIMAL_HTTP_RESPONSE_HEADDER), 0);

        // Close the connection.
        close(client_socket);
    }
    printf("Closing now...\n");
    close(server_socket);
    return 0;
}
