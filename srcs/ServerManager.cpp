#include "ServerManager.hpp"
#include <map>

/* Variables estaticas */
volatile sig_atomic_t sig = 0;
client_t ServerManager::_clients[MAX_SIP_CLIENTS] = {};
short ServerManager::_client_count = 0;


/* Funciones auxiliares estaticas */
static void    SignalHandler(int sig_num)
{
    std::cout << RED << "Signal received (" << sig_num << "). Stopping execution..."
              << RESET << std::endl;
    sig = 1;
}


static void    SetSignals()
{
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGPIPE, SIG_IGN);
}


/* Constructor y destructor */
ServerManager::ServerManager(const Socket &sip_socket, const Socket &rtp_socket)
                            : _sip_socket(sip_socket), _rtp_socket(rtp_socket) {}


ServerManager::~ServerManager() {}


/* Metodos publicos */
void    ServerManager::LaunchServer()
{
    SetSignals();

    struct pollfd fds[2];
    fds[0].fd = this->_sip_socket.fd;
    fds[0].events = POLLIN;
    fds[1].fd = this->_rtp_socket.fd;
    fds[1].events = POLLIN;

    char rtp_buffer[MAX_UDP_SIZE];
    char sip_buffer[MAX_SIP_SIZE];

	int	poll_ret = 0;
	while (!sig)
	{
		poll_ret = poll(fds, 2, TIMEOUT);
		if (poll_ret == -1)
			break ;
		if (poll_ret == 0)
			continue ;

        if (fds[0].revents & POLLIN)
        {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int recv_len = recvfrom(this->_sip_socket.fd, sip_buffer, sizeof(sip_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len == -1)
            {
                std::cerr << RED << "Error receiving SIP packet: " << strerror(errno) << RESET << std::endl;
                continue;
            }

            if (recv_len < MAX_SIP_SIZE)
                sip_buffer[recv_len] = '\0';

            if (IsEmptyBuffer(sip_buffer) == true)
            {
                std::cerr << RED << "Empty SIP message received." << RESET << std::endl;
                continue;
            }

            HandleSIP(sip_buffer, client_addr);
        }

        if (fds[1].revents & POLLIN) // RTP TO DO
        {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int recv_len = recvfrom(this->_rtp_socket.fd, rtp_buffer, sizeof(rtp_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len == -1)
            {
                std::cerr << RED << "Error receiving RTP packet: " << strerror(errno) << RESET << std::endl;
                continue;
            }

            std::cout << "Received RTP packet of size: " << recv_len << " bytes" << std::endl;
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            std::cout << GREEN << "Receiving RTP from: " << client_ip << ":" << ntohs(client_addr.sin_port);
        }
    }
}



/* Metodos privados */
void    ServerManager::HandleSIP(const char *message, const struct sockaddr_in &client_addr)
{
    std::cout << GREEN << "Received SIP message\n" << message << RESET << std::endl;
   
    try
    {
        SIPMessage sip_message;
        sip_message.ParseSIP(message);
        SIP sip(ServerManager::_clients, &ServerManager::_client_count, client_addr, this->_sip_socket, sip_message);
        sip.SIPManagement();
        PrintClients(ServerManager::_clients, ServerManager::_client_count);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}