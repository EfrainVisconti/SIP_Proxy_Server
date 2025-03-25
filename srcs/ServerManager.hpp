#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Clients.hpp"

class SIP;
class Socket;

class ServerManager
{
    private:
		static clients_t	_clients[MAX_SIP_CLIENTS];
		static short		_client_count;
		int					_sip_socket;
		int					_rtp_socket;

	public:
		ServerManager();
		~ServerManager();

		void	LaunchServer(Socket &sip_socket, Socket &rtp_socket);
        void    HandleSIP(const char *message, const struct sockaddr_in &client_addr);

};

#endif