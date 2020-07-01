#ifndef CONFIG_H
#define CONFIG_H

#define LISTEN_ADDR "0.0.0.0"
#define PORT 8123

// Minimal http ok header to reply to those webhooks which require a response.
#define MINIMAL_HTTP_RESPONSE_HEADDER "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nContent-Type: text/plain\r\n\n"

#endif
