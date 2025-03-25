#include "SIP.hpp"

SIP::SIP(clients_t *clients, short *client_count, const struct sockaddr_in &client_addr,
    int socket, const SIPMessage &msg) 
    : _clients(clients), _client_count(client_count), _client_addr(client_addr),
    _socket(socket), _msg(msg) {}


SIP::~SIP() {}


void    SIP::SIPManagement()
{
    switch (this->_msg.type)
    {
        case RESPONSE:
            ResponseCase();
            break;
        case REGISTER:
            RegisterCase();
            break;
        case INVITE:
            InviteCase();
            break;
        case BYE:
            ByeCase();
            break;
        case CANCEL:
            CancelCase();
            break;
        case ACK:
            AckCase();
            break;
        case MESSAGE:
            MessageCase();
            break;
        default:
            GenerateResponse(400, NULL);
            break;
    }
}


void    SIP::GenerateResponse(const short &code, clients_t *client)
{
    std::string message;

    if (code == 200)
        message = "OK";
    else if (code == 100)
        message = "Trying";
    else if (code == 180)
    message = "Ringing";
    else if (code == 400)
        message = "Bad Request";
    else if (code == 403)
        message = "Forbidden";
    else if (code == 404)
        message = "Not Found";
    else if (code == 486)
        message = "Busy Here";
    else if (code == 500)
        message = "Server Internal Error";
    else if (code == 401)
        message = "Unauthorized";
    else if (code == 503)
        message = "Service Unavailable";

    std::ostringstream  response;
    response << "SIP/2.0 " << code << " " << message << "\r\n";

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

    
    std::string aux = response.str();

    if (client != NULL)
    {
        if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
        (struct sockaddr *)&client->addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client->addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending response to: " << client_ip << ":" << ntohs(client->addr.sin_port);
        std::cout <<" " << client->uri << std::endl;
        std::cout << response.str() << RESET << std::endl;
    }
    else
    {
        if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
        (struct sockaddr *)&this->_client_addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");
    
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->_client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending response to: " << client_ip << ":" << ntohs(this->_client_addr.sin_port);
        std::cout <<" " << this->_msg.from << std::endl;
        std::cout << response.str() << RESET << std::endl;
    }
}


void    SIP::GenerateRequest(const std::string &method)
{
    clients_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
    if (client == NULL)
    {
        GenerateResponse(404, NULL);
        throw std::runtime_error("404 Receiver Not Found.");
    }

    if (client->status == BUSY)
    {
        GenerateResponse(486, NULL);
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

    std::string aux = request.str();

    if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
    (struct sockaddr *)&client->addr, sizeof(struct sockaddr_in)) == -1)
        throw std::runtime_error("Error sending SIP response.");

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client->addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << GREEN << "Sending request to: " << client_ip << ":" << ntohs(client->addr.sin_port);
    std::cout << " " << client->uri << std::endl;
    std::cout << request.str() << RESET << std::endl;
}


void    SIP::RegisterCase()
{
    if (FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count) == NULL)
    {
        if (AddClient(this->_clients, this->_msg.from.c_str(), this->_client_addr, this->_client_count, RECENTLY_REGISTERED) == false)
        {
            GenerateResponse(403, NULL); //Too many clients
            return;
        }
    }

    GenerateResponse(200, NULL);
}


void    SIP::InviteCase()
{
    GenerateRequest("INVITE");
    GenerateResponse(100, NULL);
    
    clients_t *current = FindClient(this->_clients, this->_msg.from.c_str(), *this->_client_count);
    if (current != NULL)
        current->status = WAITING_180;

}

void    SIP::ByeCase()
{
    std::cout << "SIP bye" << std::endl;
}

void    SIP::CancelCase()
{
    std::cout << "SIP cancel" << std::endl;
}

void    SIP::AckCase()
{
    std::cout << "SIP ack" << std::endl;
}

void    SIP::MessageCase()
{
    std::cout << "SIP message" << std::endl;
}

void    SIP::ResponseCase()
{
    if (this->_msg.response == OK)
    {
        clients_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
        if (client != NULL && client->status == WAITING_180)
        {
            GenerateResponse(200, client);
            client->status = WAITING_ACK;
        }
        return;
    }

    if (this->_msg.response == RINGING)
    {
        clients_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
        if (client != NULL)
        {
            GenerateResponse(180, client);
            client->status = WAITING_ACK;
        }
        return;
    }
}