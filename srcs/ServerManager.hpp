#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "../incs/MCXServer.hpp"
# include "../incs/Client.hpp"

class SIP;
class Socket;

/**
 * @class ServerManager
 * @brief Clase principal del programa, gestiona el bucle principal en el cual se reciben las
 * 		  comunicaciones SIP y RTP.
 * 
 * Con el método LaunchServer() se inicia el servidor.
 * Esta clase se encarga de gestionar la comunicación entre el servidor y los clientes.
 * Se encarga de recibir mensajes SIP y paquetes RTP y procesarlos.
 * Además, mantiene un registro de los clientes conectados.
 */
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