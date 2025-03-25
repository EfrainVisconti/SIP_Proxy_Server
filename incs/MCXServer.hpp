#ifndef MCXSERVER_HPP
# define MCXSERVER_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <cstring>
# include <csignal>
# include <cerrno>
# include <cstdlib>
# include <stdio.h>
# include <stdlib.h>

// Definiciones de colores para la consola
# define RED "\033[31m"
# define RESET "\033[0m"
# define GREEN "\033[32m"

# include "../srcs/ServerManager.hpp"
# include "../srcs/Socket.hpp"
# include "../srcs/SIP.hpp"
# include "../srcs/SIPMessage.hpp"

# define SIP_PORT 5060
# define RTP_PORT 4000
# define TIMEOUT 1000 // 1 segundo
# define HOST "127.0.0.1"
# define MAX_UDP_SIZE 65535
# define MAX_SIP_SIZE 1024


// clients_utils.cpp
bool       AddClient(clients_t *clients, const char *uri, const struct sockaddr_in &client_addr, \
            short *client_count, ClientStatus status);
bool        RemoveClient(clients_t *clients, const char *uri, short *client_count);
clients_t*  FindClient(clients_t *clients, const char *uri, short client_count);
void        PrintClients(clients_t *clients, short client_count);

// utils.cpp
bool        IsEmptyBuffer(const char *buffer);
void        RemoveBrackets(char *aux, const char *uri);


#endif