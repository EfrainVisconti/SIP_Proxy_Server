#include "../incs/MCXServer.hpp"

int main()
{
    try
    {
        ServerManager server_manager;
        Socket sip_socket(HOST, SIP_PORT);
        Socket rtp_socket(HOST, RTP_PORT);

        server_manager.LaunchServer(sip_socket, rtp_socket);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}