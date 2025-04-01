# include "SIP.hpp"

static std::string GetLastViaHeader(const std::string &via_header)
{
    size_t first_pos = via_header.find("\r\n");
    if (first_pos == std::string::npos)
        return via_header;

    return via_header.substr(first_pos + 2);
}


/* Metodos privados principales: SIP Cases */
/*
    100 Trying es ignorada por razones de simplicidad y eficiencia.
    Despues de un INVITE: 180 y 200 se reenvian al emisor de la llamada.

    404 Not Found se envia si el receptor no esta registrado.
*/
void    SIP::ResponseCase()
{
    client_t *client = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count); // OJO, es from no to
    if (client == NULL)
    {
        SendResponse(404, NULL); // Not Found
        return;
    }

    if (this->_msg.response == RINGING && client->status == WAITING_200)
    {
        this->_msg.via = GetLastViaHeader(this->_msg.via);
        SendResponse(180, client); // Ringing
        client->status = SENDING_ACK;
        return;
    }

    if (this->_msg.response == OK && (client->status == WAITING_200 || client->status == SENDING_ACK || client->status == SENDED_A_MESSAGE))
    {
        this->_msg.via = GetLastViaHeader(this->_msg.via);
        SendResponse(200, client);
    }
}


/*

*/
void    SIP::RegisterCase()
{
    if (FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count) == NULL)
    {
        if (AddClient(this->_clients, this->_msg.from.c_str(), this->_addr,
            this->_client_count, RECENTLY_REGISTERED) == false)
        {
            SendResponse(403, NULL); // Forbidden
            return;
        }
    }
    SendResponse(200, NULL);
}

/*

*/
void    SIP::InviteCase()
{
    client_t *current = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count);
    if (current != NULL)
        current->status = WAITING_200;

    SendResponse(100, NULL);
    SendRequest("INVITE");
}


/*

*/
void    SIP::AckCase()
{
    client_t *current = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count);
    if (current != NULL)
        current->status = CONNECTED; // Falta gestion si no se encuentra el destinatario

    SendRequest("ACK");
}


void    SIP::MessageCase()
{
    client_t *current = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count);
    if (current != NULL)
        current->status = SENDED_A_MESSAGE;

    CheckEmptyContact();
    SendRequest("MESSAGE");
}


void    SIP::CancelCase()
{
    std::cout << "SIP cancel" << std::endl;
}


void    SIP::ByeCase()
{
    std::cout << "SIP bye" << std::endl;
}