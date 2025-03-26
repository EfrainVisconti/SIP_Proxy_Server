#ifndef CLIENT_HPP
# define CLIENT_HPP

# define MAX_SIP_URI 256
# define MAX_SIP_CLIENTS 1024

enum ClientStatus
{
    RECENTLY_REGISTERED,
    WAITING_180,
    WAITING_ACK,
    BUSY,
    AVAILABLE
};

typedef struct client_s
{
    struct sockaddr_in  addr;
    char         uri[MAX_SIP_URI];
    ClientStatus status;
} client_t;

#endif