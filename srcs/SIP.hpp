#ifndef SIP_HPP
# define SIP_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Clients.hpp"

class SIPMessage;

class SIP
{
	private:
	clients_t					*_clients;
	short						*_client_count;
	const struct sockaddr_in	_client_addr;
	int							_socket;
	const SIPMessage			&_msg;

		SIP();
		void	ResponseCase();
		void	RegisterCase();
		void	InviteCase();
		void	ByeCase();
		void	CancelCase();
		void	AckCase();
		void	MessageCase();
		void	GenerateResponse(const short &code, clients_t *client);
		void	GenerateRequest(const std::string &method);

	public:
		SIP(clients_t *clients, short *client_count,
			const struct sockaddr_in &client_addr, int socket,
			const SIPMessage &msg);
		~SIP();

		void	SIPManagement();
};

#endif