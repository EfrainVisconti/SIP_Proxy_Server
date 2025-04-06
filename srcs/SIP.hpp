#ifndef SIP_HPP
# define SIP_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Client.hpp"

class SIPMessage;

/**
 * @class SIP
 * @brief Clase que encapsula la gestión del protocolo SIP.
 * 
 * Esta clase se encarga de recibir y procesar mensajes SIP, así como de registrar
 * clientes, enviar respuestas y solicitudes a los clientes. Incluye métodos para
 * manejar diferentes tipos de mensajes, como REGISTER, INVITE, ACK, MESSAGE, CANCEL y BYE.
 */
class SIP
{
	public:
		SIP(client_t *clients, short *client_count,
			const struct sockaddr_in &client_addr, const Socket &socket,
			SIPMessage &msg);
		~SIP();
		void	SIPManagement();

	private:
		client_t					*_clients;
		short						*_client_count;
		const struct sockaddr_in	_addr;
		const Socket				&_socket;
		SIPMessage					&_msg;

		SIP();
		/* Metodos privados principales: SIP Cases */
		void	ResponseCase();
		void	RegisterCase();
		void	InviteCase();
		void	AckCase();
		void	MessageCase();
		void	CancelCase();
		void	ByeCase();

		/* Metodos privados principales: SIP Send */
		void	SendResponse(const short &code, client_t *client);
		void	SendRequest(const std::string &method);

		/* Metodo privados auxiliares */
		std::string	GetSIPReasonPhrase(const short &code);
		void	SendSIPMessage(const std::string &message, const struct sockaddr_in &addr,
								const std::string &uri, const bool &is_response);
		void	CheckEmptyContact();
};

#endif