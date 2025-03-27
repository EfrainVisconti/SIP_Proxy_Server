#include "../incs/MCXServer.hpp"

int main()
{
    try
    {
        Socket sip_socket(HOST, SIP_PORT);
        Socket rtp_socket(HOST, RTP_PORT);

        ServerManager server_manager(sip_socket, rtp_socket);
        server_manager.LaunchServer();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}