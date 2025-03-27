# include "SIP.hpp"

/* Constructor y destructor */
SIP::SIP(client_t *clients, short *client_count, const struct sockaddr_in &client_addr,
    int socket, const SIPMessage &msg) 
    : _clients(clients), _client_count(client_count), _client_addr(client_addr),
    _socket(socket), _msg(msg) {}


SIP::~SIP() {}


/* Metodo publico */
/*

*/
void    SIP::SIPManagement()
{
    switch (this->_msg.type)
    {
        case RESPONSE: ResponseCase(); break;
        case REGISTER: RegisterCase(); break;
        case INVITE: InviteCase(); break;
        case ACK: AckCase(); break;
        case MESSAGE: MessageCase(); break;
        case CANCEL: CancelCase(); break;
        case BYE: ByeCase(); break;
        default: SendResponse(400, NULL); break;
    }
}


/* Metodos privados principales: SIP Send */
void    SIP::SendResponse(const short &code, client_t *client)
{
    std::string phrase = GetSIPReasonPhrase(code);
    std::ostringstream  response;
    response << "SIP/2.0 " << code << " " << phrase << "\r\n";

    if (code == 180)
    {
        std::string aux = this->_msg.via.substr(0, this->_msg.via.find("\r\n"));
        response << "Via: " << aux << "\r\n";
    }
    else
        response << "Via: " << this->_msg.via << "\r\n";

    response << "From: " << this->_msg.from_tag << "\r\n";
    response << "To: " << this->_msg.to << "\r\n";
    response << "Call-ID: " << this->_msg.call_id << "\r\n";
    response << "CSeq: " << this->_msg.cseq << "\r\n";
    response << "Contact: " << this->_msg.contact << "\r\n";
    response << "Expires: 3600\r\n";
    response << "Content-Length: " << this->_msg.content_length << "\r\n";
    response << "\r\n";

    if (!this->_msg.body.empty())
        response << this->_msg.body;

    if (client != NULL)
        SendSIPMessage(response.str(), client->addr, client->uri, true);
    else
        SendSIPMessage(response.str(), this->_client_addr, this->_msg.from, true);
}


void    SIP::SendRequest(const std::string &method)
{
    client_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
    if (client == NULL)
    {
        SendResponse(404, NULL);
        throw std::runtime_error("404 Receiver Not Found.");
    }

    if (client->status == BUSY)
    {
        SendResponse(486, NULL);
        throw std::runtime_error("486 Receiver is busy.");
    }

    client->status = WAITING_180;

    char no_brackets[MAX_SIP_URI];
    RemoveBrackets(no_brackets, client->uri);
    std::ostringstream  request;
    request << method << " " << no_brackets << " SIP/2.0\r\n";
    request << "Via: SIP/2.0/UDP " << HOST << ":" << SIP_PORT << "\r\n";
    request << "Via: " << this->_msg.via << "\r\n";
    request << "From: " << this->_msg.from_tag << "\r\n";
    request << "To: " << client->uri << "\r\n";
    request << "Call-ID: " << this->_msg.call_id << "\r\n";
    request << "CSeq: " << this->_msg.cseq << "\r\n";
    request << "Max-Forwards: 70\r\n";
    request << "Contact: " << this->_msg.from << "\r\n"; //Siempre la URI del 'caller'
    request << "Expires: 3600\r\n"; //Cambiar
    request << "Content-Length: " << this->_msg.content_length << "\r\n";
    request << "\r\n";

    if (!this->_msg.body.empty())
        request << this->_msg.body;

    SendSIPMessage(request.str(), client->addr, client->uri, false);
}


/* Metodos privados auxiliares */
/*

*/
std::string  SIP::GetSIPReasonPhrase(const short &code)
{
    if (code == 200)
        return "OK";
    if (code == 100)
        return "Trying";
    if (code == 180)
        return "Ringing";
    if (code == 400)
        return "Bad Request";
    if (code == 401)
        return "Unauthorized";
    if (code == 403)
        return "Forbidden";
    if (code == 404)
        return "Not Found";
    if (code == 486)
        return "Busy Here";
    if (code == 500)
        return "Server Internal Error";
    if (code == 503)
        return "Service Unavailable";
    return "Unknown";
}


/*

*/
void    SIP::SendSIPMessage(const std::string &message, const struct sockaddr_in &addr,
                            const std::string &uri, bool is_response)
{
    if (is_response)
    {
        if (sendto(this->_socket, message.c_str(), message.length(), 0,
        (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");
    
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending response to: " << client_ip << ":" << ntohs(addr.sin_port);
        std::cout <<" " << uri << std::endl;
        std::cout << message << RESET << std::endl;
    }
    else
    {
        if (sendto(this->_socket, message.c_str(), message.length(), 0,
        (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending request to: " << client_ip << ":" << ntohs(addr.sin_port);
        std::cout << " " << uri << std::endl;
        std::cout << message << RESET << std::endl;
    }
}
