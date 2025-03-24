#ifndef SIP_HPP
# define SIP_HPP

# include "../incs/MCXServer.hpp"

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

class SIP
{
	private:
		static clients_t	_clients[MAX_SIP_CLIENTS];
		static short		_client_count;
		SIPType		_type;
		std::string	_to;
		std::string	_from;
		std::string	_call_id;
		std::string	_cseq;
		std::string	_expires;
		std::string	_content_length;
		std::string	_body;

	public:
		SIP();
		~SIP();

		void	ParseSIP(const char *sip_buffer);
		void	PrintSIP();
		void	HandleMessage();
		void	HandleRegister();
		void	HandleInvite();
		void	HandleBye();
		void	HandleCancel();
		void	HandleAck();
		void	HandleResponse();
};


#endif