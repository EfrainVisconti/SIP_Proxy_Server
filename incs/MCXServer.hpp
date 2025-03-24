#ifndef MCXSERVER_HPP
# define MCXSERVER_HPP

# include <iostream>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <cstring>
# include <csignal>
# include <cerrno>
# include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

// Definiciones de colores para la consola
# define RED "\033[31m"
# define RESET "\033[0m"
# define GREEN "\033[32m"

# include "../srcs/ServerManager.hpp"
# include "../srcs/Socket.hpp"

#define SIP_PORT 5060
#define RTP_PORT 4000
#define TIMEOUT 1000 // 1 segundo
#define HOST "127.0.0.1"
#define MAX_UDP_SIZE 65535
#define MAX_SIP_SIZE 1024
#define MAX_SIP_CLIENTS 20

typedef struct clients_s
{
    struct sockaddr_in  addr;
    char         info[120];
} clients_t;

// clients_utils.cpp
short       AddClient(clients_t *clients, const char *info, const struct sockaddr_in *client_addr, short client_count);
bool        RemoveClient(clients_t *clients, const char *info, short *client_count);
clients_t*  FindClient(clients_t *clients, const char *info, short client_count);
void        PrintClients(clients_t *clients, short client_count);


#endif