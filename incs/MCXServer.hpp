#ifndef MCXSERVER_HPP
# define MCXSERVER_HPP

// Standard headers
# include <cstdlib>
# include <cstdio>
# include <iostream>
# include <cstring>
# include <cerrno>
# include <string>
# include <sstream>

// System headers
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <fcntl.h>
# include <signal.h>

// Headers del proyecto
# include "../srcs/ServerManager.hpp"
# include "../srcs/Socket.hpp"
# include "../srcs/SIP.hpp"
# include "../srcs/SIPMessage.hpp"

// Defines de colores para la consola
# define RED "\033[31m"
# define RESET "\033[0m"
# define GREEN "\033[32m"

// Defines auxiliares para el servidor
# define HOST "127.0.0.1"
# define SIP_PORT 5060
# define RTP_PORT 4000
# define TIMEOUT 1000 // 1 segundo
# define MAX_UDP_SIZE 65535
# define MAX_SIP_SIZE 1024

// Declaracion de funciones por archivo
// Clients_utils.cpp
bool       AddClient(client_t *clients, const char *uri, const struct sockaddr_in &client_addr, \
            short *client_count, ClientStatus status);
bool        RemoveClient(client_t *clients, const char *uri, short *client_count);
client_t*  FindClient(client_t *clients, const char *uri, short client_count);
void        PrintClients(client_t *clients, short client_count);

// utils.cpp
bool        IsEmptyBuffer(const char *buffer);
void        RemoveBrackets(char *aux, const char *uri);

#endif