#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "../incs/MCXServer.hpp"

/**
 * @class Socket
 * @brief Clase que representa un socket UDP.
 * 
 * Esta clase encapsula la funcionalidad de un socket UDP, incluyendo la creación,
 * configuración y cierre del socket.
 */
class Socket
{
	public:
		uint32_t            host;
		uint16_t            port;
        struct sockaddr_in  socket_address;
        int                 fd;

        Socket();
		Socket(const char *host, const uint16_t &port);
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		
	private:
        void    CreateSocket();
		void	SetSockaddr_in();
};

#endif