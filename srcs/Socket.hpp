#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "../incs/MCXServer.hpp"

class Socket
{
	public:
		uint32_t            host;
		uint16_t            port;
        struct sockaddr_in  socket_address;
        int                 fd;

		Socket(const char *host, const uint16_t &port);
		~Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		
	private:
        Socket();
        void    CreateSocket();
		void	SetSockaddr_in();
};

#endif