# include "SIP.hpp"

/* Constructor y destructor */
SIP::SIP(client_t *clients, short *client_count, const struct sockaddr_in &_addr,
    const Socket &socket, SIPMessage &msg) 
    : _clients(clients), _client_count(client_count), _addr(_addr),
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
        default: throw std::runtime_error("Unknown SIP message type.");
    }
}


/* Metodos privados principales: SIP Send */
void    SIP::SendResponse(const short &code, client_t *client)
{
    std::string phrase = GetSIPReasonPhrase(code);
    std::ostringstream  response;
    response << "SIP/2.0 " << code << " " << phrase << "\r\n";
    response << "Via: " << this->_msg.via << "\r\n"; // Revisar
    response << "From: " << this->_msg.from_tag << "\r\n";
    response << "To: " << this->_msg.to << "\r\n"; //Revisar gestión tag
    response << "Call-ID: " << this->_msg.call_id << "\r\n";
    response << "CSeq: " << this->_msg.cseq << "\r\n";
    // response << "Contact: " << this->_msg.contact << "\r\n";
    
    if (this->_msg.expires != 0)
    	response << "Expires: " << this->_msg.expires << "\r\n";

    if (!this->_msg.body.empty() && code != 100)
    {
        response << "Content-Length: " << this->_msg.content_length << "\r\n";
        response << "\r\n";
        response << this->_msg.body;
    }
    else
        response << "\r\n";

    if (client != NULL)
        SendSIPMessage(response.str(), client->addr, client->uri, true);
    else
        SendSIPMessage(response.str(), this->_addr, this->_msg.from, true);
}


void    SIP::SendRequest(const std::string &method)
{
    client_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
    if (client == NULL)
    {
        SendResponse(404, NULL); // Not Found
        std::cout << this->_msg.to << std::endl;
        std::cout << this->_msg.from << std::endl;
        throw std::runtime_error("404 Receiver Not Found.");
    }

    
    char no_brackets[MAX_SIP_URI];
    RemoveBrackets(no_brackets, client->uri);
    
    std::ostringstream  request;
    request << method << " " << no_brackets << " SIP/2.0\r\n";
    
    if (method == "INVITE") // De momento
    {
        client->status = RESPONDING_TO_INVITE;
        char socket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->_socket.socket_address.sin_addr), socket_ip, INET_ADDRSTRLEN);
        request << "Via: SIP/2.0/UDP " << socket_ip << ":"
                  << ntohs(this->_socket.socket_address.sin_port) << ";branch=z9hG4bK-server\r\n";
    }

    request << "Via: " << this->_msg.via << "\r\n";
    request << "From: " << this->_msg.from_tag << "\r\n";
    request << "To: " << this->_msg.to_tag << "\r\n";
    request << "Call-ID: " << this->_msg.call_id << "\r\n";
    request << "CSeq: " << this->_msg.cseq << "\r\n";
    request << "Max-Forwards: 70\r\n";
    request << "Contact: " << this->_msg.contact << "\r\n"; // Siempre la URI del 'caller'
    if (this->_msg.expires != 0)
        request << "Expires: " << this->_msg.expires << "\r\n";

    if (!this->_msg.content_type.empty())
        request << "Content-Type: " << this->_msg.content_type << "\r\n";
    request << "Content-Length: " << this->_msg.content_length << "\r\n";
    request << "\r\n";

    if (!this->_msg.body.empty())
    {
        request << this->_msg.body;
        request << "\r\n";
    }

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
        if (sendto(this->_socket.fd, message.c_str(), message.length(), 0,
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
        if (sendto(this->_socket.fd, message.c_str(), message.length(), 0,
        (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending request to: " << client_ip << ":" << ntohs(addr.sin_port);
        std::cout << " " << uri << std::endl;
        std::cout << message << RESET << std::endl;
    }
}


void    SIP::CheckEmptyContact()
{
    if (this->_msg.contact.empty())
    {
        std::string port = std::to_string(ntohs(this->_addr.sin_port));

        if (this->_msg.from.front() == '<' && this->_msg.from.back() == '>')
        {
            std::string aux = this->_msg.from.substr(0, this->_msg.from.size() - 1) + ":" + port + ">";
            this->_msg.contact = aux;
        }
        else
        {
            std::string aux = "<" + this->_msg.from + ":" + port + ">";
            this->_msg.contact = aux;
        }
    }
}
