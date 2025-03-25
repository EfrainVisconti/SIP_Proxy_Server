#ifndef SIP_HPP
# define SIP_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Clients.hpp"

enum SIPType
{
    RESPONSE,
    REGISTER,
    INVITE,
    BYE,
    CANCEL,
    ACK,
    UNKNOWN
};

enum SIPResponse
{
	INVALID,
	OK,
	TRYING,
	RINGING,
	BAD_REQUEST,
	UNAUTHORIZED,
	FORBIDDEN,
	NOT_FOUND,
	REQUEST_TIMEOUT,
	GONE,
	REQUEST_ENTITY_TOO_LARGE,
	BUSY_HERE
};

class SIP
{
	private:
		SIPType		_type;
		SIPResponse	_response;
		std::string	_via;
		std::string	_to;
		std::string	_from;
		std::string	_from_tag;
		std::string	_call_id;
		std::string	_cseq;
		std::string	_contact;
		std::string	_content_length;
		std::string	_body;
		clients_t	*_clients;
		short		*_client_count;
		const struct sockaddr_in	_client_addr;
		int			_socket;

		SIP();
		SIPType	GetMessageType(const std::string &message);
		void	SetResponse(const std::string &response);
		void	ResponseCase();
		void	RegisterCase();
		void	InviteCase();
		void	ByeCase();
		void	CancelCase();
		void	AckCase();
		void	GenerateResponse(const short &code, clients_t *client);
		void	GenerateRequest(const std::string &method);

	public:
		SIP(clients_t *clients, short *client_count, const struct sockaddr_in &client_addr, int socket);
		~SIP();

		void	ParseSIP(const char *sip_buffer);
		void	SIPManagement();
};

#endif