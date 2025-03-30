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
// void    ServerManager::LaunchServer()
// {
//     SetSignals();

//     struct pollfd fds[2];
//     fds[0].fd = this->_sip_socket.fd;
//     fds[0].events = POLLIN;
//     fds[1].fd = this->_rtp_socket.fd;
//     fds[1].events = POLLIN;

//     char rtp_buffer[MAX_UDP_SIZE];
//     char sip_buffer[MAX_SIP_SIZE];

// 	int	poll_ret = 0;
// 	while (!sig)
// 	{
// 		poll_ret = poll(fds, 2, TIMEOUT);
// 		if (poll_ret == -1)
// 			break ;
// 		if (poll_ret == 0)
// 			continue ;

//         if (fds[0].revents & POLLIN)
//         {
//             struct sockaddr_in client_addr;
//             socklen_t addr_len = sizeof(client_addr);

//             int recv_len = recvfrom(this->_sip_socket.fd, sip_buffer, sizeof(sip_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
//             if (recv_len == -1)
//             {
//                 std::cerr << RED << "Error receiving SIP packet: " << strerror(errno) << RESET << std::endl;
//                 continue;
//             }

//             if (recv_len < MAX_SIP_SIZE)
//                 sip_buffer[recv_len] = '\0';

//             if (IsEmptyBuffer(sip_buffer) == true)
//             {
//                 std::cerr << RED << "Empty SIP message received." << RESET << std::endl;
//                 continue;
//             }

//             HandleSIP(sip_buffer, client_addr);
//         }

//         if (fds[1].revents & POLLIN) // RTP
//         {
//             struct sockaddr_in client_addr;
//             socklen_t addr_len = sizeof(client_addr);

//             int recv_len = recvfrom(this->_rtp_socket.fd, rtp_buffer, sizeof(rtp_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
//             if (recv_len == -1)
//             {
//                 std::cerr << RED << "Error receiving RTP packet: " << strerror(errno) << RESET << std::endl;
//                 continue;
//             }

//             std::cout << "Received RTP packet of size: " << recv_len << " bytes" << std::endl;
//             char client_ip[INET_ADDRSTRLEN];
//             inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
//             std::cout << GREEN << "Receiving RTP from: " << client_ip << ":" << ntohs(client_addr.sin_port);
//             std::cout << "RTP message: " << rtp_buffer << std::endl;
//         }
//     }
// }
void ServerManager::LaunchServer()
{
    SetSignals();

    struct pollfd fds[2];
    fds[0].fd = this->_sip_socket.fd;
    fds[0].events = POLLIN;
    fds[1].fd = this->_rtp_socket.fd;
    fds[1].events = POLLIN;

    char rtp_buffer[MAX_UDP_SIZE];
    char sip_buffer[MAX_SIP_SIZE];

    // Mapeo de direcciones IP y puertos RTP
    std::map<std::string, sockaddr_in> rtp_destinations;

    int poll_ret = 0;
    while (!sig)
    {
        poll_ret = poll(fds, 2, TIMEOUT);
        if (poll_ret == -1)
            break;
        if (poll_ret == 0)
            continue;

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

        if (fds[1].revents & POLLIN) // RTP
        {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int recv_len = recvfrom(this->_rtp_socket.fd, rtp_buffer, sizeof(rtp_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len == -1)
            {
                std::cerr << RED << "Error receiving RTP packet: " << strerror(errno) << RESET << std::endl;
                continue;
            }

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(client_addr.sin_port);

            std::cout << GREEN << "Receiving RTP from: " << client_ip << ":" << client_port << RESET << std::endl;

            std::string sender_key = std::string(client_ip) + ":" + std::to_string(client_port);
            if (rtp_destinations.find(sender_key) == rtp_destinations.end())
            {
                sockaddr_in destination;
                destination.sin_family = AF_INET;
                
                if (strcmp(client_ip, "192.168.1.94") == 0) // Linphone Ubuntu
                {
                    inet_pton(AF_INET, "192.168.1.50", &destination.sin_addr);
                    destination.sin_port = htons(7081);
                }
                else if (strcmp(client_ip, "192.168.1.50") == 0) // Linphone Windows
                {
                    inet_pton(AF_INET, "192.168.1.94", &destination.sin_addr);
                    destination.sin_port = htons(7081);
                }
                else
                {
                    std::cerr << RED << "Unknown RTP sender. Dropping packet." << RESET << std::endl;
                    continue;
                }

                rtp_destinations[sender_key] = destination;
            }

            sockaddr_in dest_addr = rtp_destinations[sender_key];

            int sent_len = sendto(this->_rtp_socket.fd, rtp_buffer, recv_len, 0,
                                  (struct sockaddr *)&dest_addr, sizeof(dest_addr));

            if (sent_len == -1)
                std::cerr << RED << "Error forwarding RTP packet: " << strerror(errno) << RESET << std::endl;
            else
            {
                std::cout << GREEN << "Forwarded RTP packet to: " 
                          << inet_ntoa(dest_addr.sin_addr) << ":" << ntohs(dest_addr.sin_port) 
                          << " (size: " << sent_len << " bytes)" << RESET << std::endl;
            }
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