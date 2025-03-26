# include "SIP.hpp"

/* Metodos privados principales: SIP Cases */
void    SIP::ResponseCase()
{
    if (this->_msg.response == OK)
    {
        client_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
        if (client != NULL && client->status == WAITING_180)
        {
            SendResponse(200, client);
            client->status = WAITING_ACK;
        }
        return;
    }

    if (this->_msg.response == RINGING)
    {
        client_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
        if (client != NULL)
        {
            SendResponse(180, client);
            client->status = WAITING_ACK;
        }
        return;
    }
}


void    SIP::RegisterCase()
{
    if (FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count) == NULL)
    {
        if (AddClient(this->_clients, this->_msg.from.c_str(), this->_client_addr, this->_client_count, RECENTLY_REGISTERED) == false)
        {
            SendResponse(403, NULL); //Too many clients
            return;
        }
    }

    SendResponse(200, NULL);
}


void    SIP::InviteCase()
{
    SendRequest("INVITE");
    SendResponse(100, NULL);
    
    client_t *current = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count);
    if (current != NULL)
        current->status = WAITING_180;

}


void    SIP::AckCase()
{
    std::cout << "SIP ack" << std::endl;
}


void    SIP::MessageCase()
{
    std::cout << "SIP message" << std::endl;
}


void    SIP::CancelCase()
{
    std::cout << "SIP cancel" << std::endl;
}


void    SIP::ByeCase()
{
    std::cout << "SIP bye" << std::endl;
}