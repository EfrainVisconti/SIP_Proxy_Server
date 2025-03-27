#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Client.hpp"

class SIP;
class Socket;

class ServerManager
{
    private:
		static client_t		_clients[MAX_SIP_CLIENTS];
		static short		_client_count;
		const Socket				&_sip_socket;
		const Socket				&_rtp_socket;

		ServerManager();
		void    HandleSIP(const char *message, const struct sockaddr_in &client_addr);

	public:
		ServerManager(const Socket &sip_socket, const Socket &rtp_socket);
		~ServerManager();

		void	LaunchServer();
};

#endif