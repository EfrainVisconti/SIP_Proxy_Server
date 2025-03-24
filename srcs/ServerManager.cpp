#include "ServerManager.hpp"
# include "SIP.hpp"

volatile sig_atomic_t sig = 0;

ServerManager::ServerManager()
{

}


ServerManager::~ServerManager()
{

}


static void    SignalHandler(int sig_num)
{
    std::cout << RED << "Signal received (" << sig_num << "). Stopping execution..." << RESET << std::endl;
    sig = 1;
}


static void    SetSignals()
{
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGPIPE, SIG_IGN);
}


void    ServerManager::HandleSIP(const char *message, const struct sockaddr_in *client_addr)
{
    std::cout << GREEN << "Received SIP packet" << RESET << std::endl;
   
    try
    {
        SIP sip;
        sip.ParseSIP(message);
        sip.HandleMessage();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}


void    ServerManager::LaunchServer(Socket &sip_socket, Socket &rtp_socket)
{
    SetSignals();

    struct pollfd fds[2];
    fds[0].fd = sip_socket.fd;
    fds[0].events = POLLIN;
    fds[1].fd = rtp_socket.fd;
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

            int recv_len = recvfrom(sip_socket.fd, sip_buffer, sizeof(sip_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len == -1)
            {
                std::cerr << RED << "Error receiving SIP packet: " << strerror(errno) << RESET << std::endl;
                continue;
            }

            if (recv_len < MAX_SIP_SIZE)
                sip_buffer[recv_len] = '\0';

            HandleSIP(sip_buffer, &client_addr);
        }

        if (fds[1].revents & POLLIN) // RTP
        {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int recv_len = recvfrom(rtp_socket.fd, rtp_buffer, sizeof(rtp_buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
            if (recv_len == -1)
            {
                std::cerr << RED << "Error receiving RTP packet: " << strerror(errno) << RESET << std::endl;
                continue;
            }

            std::cout << "Received RTP packet of size: " << recv_len << " bytes" << std::endl;
        }
    }
}