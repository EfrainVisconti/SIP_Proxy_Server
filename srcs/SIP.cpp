#include "SIP.hpp"

clients_t	_clients[MAX_SIP_CLIENTS] = {};
short		_client_count = 0;

SIP::SIP()
{

}


SIP::~SIP()
{

}


static SIPType GetMessageType(const std::string &message)
{
    if (message.find("SIP") == 0)
        return RESPONSE;
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
    if (sip_buffer == NULL || sip_buffer[0] == '\0')
        throw std::runtime_error("Invalid SIP packet");

    std::string message(sip_buffer);

    this->_type = GetMessageType(message);

    if (this->_type == UNKNOWN)
        throw std::runtime_error("Invalid SIP message type");

    this->_to = GetHeader(message, "To: ");
    this->_from = GetHeader(message, "From: ");
    this->_call_id = GetHeader(message, "Call-ID: ");
    this->_cseq = GetHeader(message, "CSeq: ");
    this->_expires = GetHeader(message, "Expires: ");
    this->_content_length = GetHeader(message, "Content-Length: ");
    this->_body = GetBody(message);
}

void    SIP::PrintSIP()
{
    std::cout << "Message Type: " << this->_type << std::endl;
    std::cout << "To: " << this->_to << std::endl;
    std::cout << "From: " << this->_from << std::endl;
    std::cout << "Call-ID: " << this->_call_id << std::endl;
    std::cout << "CSeq: " << this->_cseq << std::endl;
    std::cout << "Expires: " << this->_expires << std::endl;
    std::cout << "Content-Length: " << this->_content_length << std::endl;
    std::cout << "Body: " << this->_body << std::endl;
}

void    SIP::HandleMessage()
{
    switch (this->_type)
    {
        case RESPONSE:
            HandleResponse();
            break;
        case REGISTER:
            HandleRegister();
            break;
        case INVITE:
            HandleInvite();
            break;
        case BYE:
            HandleBye();
            break;
        case CANCEL:
            HandleCancel();
            break;
        case ACK:
            HandleAck();
            break;
        default:
            throw std::runtime_error("Invalid SIP message type");
    }
}


void    SIP::HandleRegister()
{
    
}

void    SIP::HandleInvite()
{
    std::cout << "SIP invited" << std::endl;
}

void    SIP::HandleBye()
{
    std::cout << "SIP bye" << std::endl;
}

void    SIP::HandleCancel()
{
    std::cout << "SIP cancel" << std::endl;
}

void    SIP::HandleAck()
{
    std::cout << "SIP ack" << std::endl;
}

void    SIP::HandleResponse()
{
    std::cout << "SIP response" << std::endl;
}