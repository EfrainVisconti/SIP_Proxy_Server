#include "SIP.hpp"

SIP::SIP(clients_t *clients, short *client_count, const struct sockaddr_in &client_addr, int socket) 
    : _clients(clients), _client_count(client_count), _client_addr(client_addr), _socket(socket) {}


SIP::~SIP() {}


void SIP::SetResponse(const std::string &response)
{
    switch (std::stoi(response))
    {
        case 200:
            this->_response = OK;
            break;
        case 100:
            this->_response = TRYING;
            break;
        case 180:
            this->_response = RINGING;
            break;
        case 400:
            this->_response = BAD_REQUEST;
            break;
        case 401:
            this->_response = UNAUTHORIZED;
            break;
        case 403:
            this->_response = FORBIDDEN;
            break;
        case 404:
            this->_response = NOT_FOUND;
            break;
        case 408:
            this->_response = REQUEST_TIMEOUT;
            break;
        case 410:
            this->_response = GONE;
            break;
        case 413:
            this->_response = REQUEST_ENTITY_TOO_LARGE;
            break;
        case 486:
            this->_response = BUSY_HERE;
            break;
        default:
            this->_response = INVALID;
            break;
    }
}


SIPType SIP::GetMessageType(const std::string &message)
{
    if (message.find("SIP") == 0)
    {
        SetResponse(message.substr(8, 3));
        return RESPONSE;
    }
    if (message.find("REGISTER") == 0)
        return REGISTER;
    if (message.find("INVITE") == 0)
        return INVITE;
    if (message.find("BYE") == 0)
        return BYE;
    if (message.find("CANCEL") == 0)
        return CANCEL;
    if (message.find("ACK") == 0)
        return ACK;
    return UNKNOWN;
}


static std::string GetHeader(const std::string &message, const std::string &header)
{
    size_t start = message.find(header);
    if (start == std::string::npos)
        return "";

    start += header.length();
    if (header == "Via :")
    {
        size_t end = message.find("\r\n", start);
        std::string aux1 = message.substr(start, end - start);
        start = end + 2;
        end = message.find("\r\n", start);
        std::string aux2 = message.substr(start, end - start);
        return aux1 + "\r\n" + aux2;
    }

    size_t end = message.find("\r\n", start);
    return message.substr(start, end - start);
}


static std::string GetBody(const std::string &message)
{
    size_t start = message.find("\r\n\r\n");

    if (start == std::string::npos)
        return "";

    start += 4;
    return message.substr(start);
}


void    SIP::ParseSIP(const char *sip_buffer)
{
    std::string message(sip_buffer);

    this->_type = GetMessageType(message);

    if (this->_type == UNKNOWN)
        throw std::runtime_error("Invalid SIP message.");
    
    this->_from_tag = GetHeader(message, "From: ");
    if (this->_from_tag.empty())
    {
        GenerateResponse(400, NULL);
        return;
    }

    this->_from = this->_from_tag.substr(0, this->_from_tag.find(";"));

    this->_to = GetHeader(message, "To: ");
    if (this->_to.empty())
    {
        GenerateResponse(400, NULL);
        return;
    }

    this->_via = GetHeader(message, "Via: ");
    this->_call_id = GetHeader(message, "Call-ID: ");
    this->_cseq = GetHeader(message, "CSeq: ");
    this->_contact = GetHeader(message, "Contact: ");
    this->_content_length = GetHeader(message, "Content-Length: ");
    this->_body = GetBody(message);
}


void    SIP::SIPManagement()
{
    switch (this->_type)
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
        std::string aux = this->_via.substr(0, this->_via.find("\r\n"));
        response << "Via: " << aux << "\r\n";
    }
    else
        response << "Via: " << this->_via << "\r\n";

    response << "From: " << this->_from_tag << "\r\n";
    response << "To: " << this->_to << "\r\n";
    response << "Call-ID: " << this->_call_id << "\r\n";
    response << "CSeq: " << this->_cseq << "\r\n";
    response << "Contact: " << this->_contact << "\r\n";
    response << "Expires: 3600\r\n";
    response << "Content-Length: " << this->_content_length << "\r\n";
    response << "\r\n";

    if (!this->_body.empty())
        response << this->_body;

    
    std::string aux = response.str();

    if (client != NULL)
    {
        if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
        (struct sockaddr *)&client->addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client->addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending response to: " << client_ip << ":" << ntohs(client->addr.sin_port) << std::endl;
        std::cout << response.str() << RESET << std::endl;
    }
    else
    {
        if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
        (struct sockaddr *)&this->_client_addr, sizeof(struct sockaddr_in)) == -1)
            throw std::runtime_error("Error sending SIP response.");
    
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->_client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending response to: " << client_ip << ":" << ntohs(this->_client_addr.sin_port) << std::endl;
        std::cout << response.str() << RESET << std::endl;
    }
}


void    SIP::GenerateRequest(const std::string &method)
{
    std::ostringstream  request;
    request << method << " sip:127.0.0.1 SIP/2.0\r\n";
    request << "Via: SIP/2.0/UDP " << HOST << ":" << SIP_PORT << "\r\n";
    request << "Via: " << this->_via << "\r\n";
    request << "From: " << this->_from_tag << "\r\n";

    clients_t *client = FindClient(this->_clients, this->_to.c_str(), *this->_client_count);
    if (client == NULL)
    {
        GenerateResponse(404, NULL);
        return;
    }

    if (client->status == BUSY)
        GenerateResponse(486, NULL);

    client->status = WAITING_180;
    request << "To: " << client->info << "\r\n";
    request << "Call-ID: " << this->_call_id << "\r\n";
    request << "CSeq: " << this->_cseq << "\r\n";
    request << "Contact: " << client->info << "\r\n";
    request << "Expires: 3600\r\n";
    request << "Content-Length: " << this->_content_length << "\r\n";
    request << "\r\n";

    if (!this->_body.empty())
        request << this->_body;

    std::string aux = request.str();
    if (sendto(this->_socket, aux.c_str(), aux.length(), 0,
    (struct sockaddr *)&this->_client_addr, sizeof(struct sockaddr_in)) == -1)
        throw std::runtime_error("Error sending SIP request.");

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(this->_client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << GREEN << "Sending request to: " << client_ip << ":" << ntohs(this->_client_addr.sin_port) << std::endl;
    std::cout << request.str() << RESET << std::endl;
}


void    SIP::RegisterCase()
{
    if (FindClient(this->_clients, this->_from.c_str(), *this->_client_count) == NULL)
    {
        if (AddClient(this->_clients, this->_from.c_str(), this->_client_addr, this->_client_count, RECENTLY_REGISTERED) == false)
        {
            GenerateResponse(503, NULL);
            return;
        }
    }

    GenerateResponse(200, NULL);
}


void    SIP::InviteCase()
{
    GenerateRequest("INVITE");
    GenerateResponse(100, NULL);

    clients_t *current = FindClient(this->_clients, this->_from.c_str(), *this->_client_count);
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

void    SIP::ResponseCase()
{
    if (this->_response == OK)
    {
        clients_t *client = FindClient(this->_clients, this->_to.c_str(), *this->_client_count);
        if (client != NULL && client->status == WAITING_180)
        {
            GenerateResponse(200, client);
            client->status = WAITING_ACK;
        }
        return;
    }

    if (this->_response == RINGING)
    {
        clients_t *client = FindClient(this->_clients, this->_to.c_str(), *this->_client_count);
        if (client != NULL)
        {
            GenerateResponse(180, client);
            client->status = WAITING_ACK;
        }
        return;
    }
}
