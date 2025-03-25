#ifndef CLIENTS_HPP
# define CLIENTS_HPP

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
    char         info[128];
    ClientStatus status;
} clients_t;

#endif