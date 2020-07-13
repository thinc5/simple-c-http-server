#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>
#include <netdb.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"
#include "http.h"
#include "filter.h"

typedef enum
{
    CONTINUE,
    FINISHED,
    NO_DATA_READ,
    MANUAL_INTERRUPT,
    UNEXPECTED_CLOSE,
    INVALID_DATA,
} READ_STATES;

volatile sig_atomic_t running = true;

void show_server_address(struct sockaddr_in *server_address);
void display_peer_ip(struct sockaddr *client_address);

READ_STATES read_single_http_message(int s, HTTP_REQUEST *req);

/**
 * Signal handler, catches sigint and terminates the server's
 * socket connection.
 */
void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
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
    address.sin_addr.s_addr = inet_addr(LISTEN_ADDR);
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
        // Accept any incoming connections.
        struct sockaddr_in client_address;
        socklen_t sock_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &sock_size);

        printf("------ Connection established ------\n");

        // Read incoming connection's ip address.
        display_peer_ip((struct sockaddr *)&client_address.sin_addr);

        // We need to keep reading from this socket until we encounter an error, timeout or "Connection: close"
        bool connection = true;
        while (connection)
        {
            // Read and validate data if any is avaliable.
            HTTP_REQUEST request = {0};
            READ_STATES read_state = read_single_http_message(client_socket, &request);
            switch (read_state)
            {
            case FINISHED:
                break;
            case NO_DATA_READ:
                // printf("No data read from socket, closing connection\n");
            case MANUAL_INTERRUPT:
            case UNEXPECTED_CLOSE:
            case INVALID_DATA:
                printf("Packet was invalid or process was interrupted %d.\n", read_state);
                connection = false;
                continue;
            default:
                break;
            }

            // Show the breakdown of the http request.
            show_http_request(&request);

            // Filter data.
            filter_request(request);

            // Action data.
            send(client_socket, MINIMAL_HTTP_RESPONSE_HEADDER, sizeof(MINIMAL_HTTP_RESPONSE_HEADDER), 0);

            // Are we keep-alive?
            if (strcmp(get_http_header_value(request, "Connection"), "keep-alive") != 0)
            {
                connection = false;
            }
            else
            {
                printf("Keep alive!\n");
            }

            // Free data.
            free_http_request(&request);
        }

        // Close the connection.
        close(client_socket);
        client_socket = -1;

        printf("------ Connection closed ------\n");
    }

    printf("Exiting now...\n");
    close(server_socket);
    return 0;
}

/**
 * Continuously read from a provided socket until we read
 * 0 bytes from it. Buffer is copied into an allocated string.
 */
READ_STATES read_single_http_message(int s, HTTP_REQUEST *req)
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
    struct timespec timeout = {.tv_nsec = 3000};
    pselect(s + 1, &fds, NULL, NULL, &timeout, NULL);
    fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK);

    // Actually read from the socket into the buffer.
    // Temporary buffer.
    char temp_buff[DEFAULT_PAYLOAD_SIZE];
    ssize_t bytes_read = 0;

    // Full perminent buffer.
    unsigned long buff_size = DEFAULT_PAYLOAD_SIZE;
    char *big_buff = (char *)malloc(buff_size * sizeof(char));
    memset(big_buff, '\0', buff_size);
    ssize_t total_bytes_read = 0;

    // State to keep track of connection.
    READ_STATES state = CONTINUE;
    while (state == CONTINUE)
    {
        // Have we been interupted mid read?
        if (!running)
        {
            state = MANUAL_INTERRUPT;
            continue;
        }

        // Read into temporary buffer.
        memset(temp_buff, '\0', DEFAULT_PAYLOAD_SIZE);
        bytes_read = recv(s, temp_buff, DEFAULT_PAYLOAD_SIZE, 0);

        // Did we disconnect?
        if (bytes_read == -1)
        {
            state = UNEXPECTED_CLOSE;
            continue;
        }

        // Was there anything to read?
        if (bytes_read == 0)
        {
            state = CONTINUE;
            continue;
        }

        // Do we need to reallocate more memory?
        if (buff_size < total_bytes_read + bytes_read)
        {
            big_buff = realloc(big_buff, buff_size + DEFAULT_PAYLOAD_SIZE);
            memset(big_buff + buff_size, '\0', DEFAULT_PAYLOAD_SIZE);
            buff_size = buff_size + DEFAULT_PAYLOAD_SIZE;
        }
        // Copy data from temp buffer to the array.
        memcpy(big_buff + total_bytes_read, temp_buff, bytes_read);
        // Update buffer total size (bytes read).
        total_bytes_read = total_bytes_read + bytes_read;
    }

    printf("Bytes read: %lu\nBody:\n%s\n", total_bytes_read, big_buff);
    // Process and parse the request.
    if (total_bytes_read < 1)
    {
        printf("no data read?\n");
        state = NO_DATA_READ;
    }
    else if (!parse_http_request(req, big_buff))
    {
        state = UNKNOWN_DATA;
    }

    free(big_buff);
    return state;
}

// Debug/Helper functions.

/**
 * A simple function to show the server address, given a refernce to a
 * sockaddr_in struct.
 */
void show_server_address(struct sockaddr_in *server_address)
{
    char host_buffer[INET6_ADDRSTRLEN];
    char port_buffer[NI_MAXSERV];
    socklen_t addr_len = sizeof(*server_address);
    getnameinfo((struct sockaddr *)server_address, addr_len,
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
