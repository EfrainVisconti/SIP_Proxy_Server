#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# define MAX_SIP_URI 256
# define MAX_SIP_CLIENTS 10

enum ClientStatus
{
    RECENTLY_REGISTERED,
    WAITING_180,
    WAITING_ACK,
    BUSY,
    AVAILABLE
};

typedef struct clients_s
{
    struct sockaddr_in  addr;
    char         uri[MAX_SIP_URI];
    ClientStatus status;
} clients_t;

#endif