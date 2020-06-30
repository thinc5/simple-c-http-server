#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>


#include "config.h"

const char http_ok_header[8000] = "HTTP/1.1 200 OK\r\n\nContent-Length: 2\r\n\n"
	"Content-Type: text/plain\r\n\n\r\n\nok";

/**
 * A simple function to show the server address, given a refrence to a
 * sockaddr_in struct.
 */
void show_server_address(struct sockaddr_in *server_address)
{
    char host_buffer[INET6_ADDRSTRLEN];
    char port_buffer[NI_MAXSERV];
    socklen_t addr_len = sizeof(*server_address);
    int err = getnameinfo((struct sockaddr *) server_address, addr_len,
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
 * Continuously read from a provided socket until we read
 * 0 bytes from it. Buffer is copied into an allocated string.
 */
void read_peer_data(int s)
{
	char buf[4096];
	ssize_t bytes_read;
	do {
     		bytes_read = recv(s, buf, sizeof(buf), 0);
     		if (bytes_read > 0) {
			printf("Chunk:\n%s\n", buf);
     		}
	} while (bytes_read > 0);	
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
    int port = PORT;

    // Create a socket, using ipv4 and tcp.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // Specify the address to bind to.
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(inet_addr("0.0.0.0"));
    // Bind the socket to the address.
    bind(server_socket, (struct sockaddr *) &address, sizeof(address));
    // Attempt to listen on bound socket.
    listen(server_socket, 20);
    // Show the address we are listening on.
    show_server_address(&address);
    
    // Handle all incoming connections.
    int client_socket;
    while(1) {
	// Accept incoming connection.
	struct sockaddr_in client_address;
	socklen_t sock_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *) &client_address, &sock_size);
        // Read inmcomming connection ip address.
	display_peer_ip((struct sockaddr *)&client_address.sin_addr);
	// Read data if any is avaliable.
	read_peer_data(client_socket);
        // Close the connection.
	close(client_socket);
    }

    return 0;
}

