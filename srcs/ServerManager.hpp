#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "../incs/MCXServer.hpp"

class SIP;
class Socket;

class ServerManager
{
    private:

	public:
		ServerManager();
		~ServerManager();

		void	LaunchServer(Socket &sip_socket, Socket &rtp_socket);
        void    HandleSIP(const char *message, const struct sockaddr_in *client_addr);

};


#endif