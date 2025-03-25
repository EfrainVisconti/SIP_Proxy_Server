#include "SIPMessage.hpp"

SIPMessage::SIPMessage ()
{
    this->type = UNKNOWN;
    this->response = OTHER;
    this->expires = 0;
    this->content_length = 0;
}


SIPMessage::~SIPMessage() {}


static std::string  GetHeader(const std::string &message, const std::string &header)
{
    size_t start = message.find(header);
    if (start == std::string::npos)
        return "";

    start += header.length();

    if (header == "Via :") //REVISAR ESTO
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


static std::string  GetBody(const std::string &message)
{
    size_t start = message.find("\r\n\r\n");

    if (start == std::string::npos)
        return "";

    start += 4;
    return message.substr(start);
}


void    SIPMessage::SetResponse(const std::string &message)
{
    size_t  start = message.find(' ');
    if (start == std::string::npos)
        throw std::runtime_error("Invalid SIP message format.");

    size_t end = message.find(' ', start + 1);
    if (end == std::string::npos)
        throw std::runtime_error("Invalid SIP message format.");

    std::string status_code = message.substr(start + 1, end - start - 1);

    switch (std::stoi(status_code))
    {
        case 200: this->response = OK; break;
        case 100: this->response = TRYING; break;
        case 180: this->response = RINGING; break;
        case 400: this->response = BAD_REQUEST; break;
        case 401: this->response = UNAUTHORIZED; break;
        case 403: this->response = FORBIDDEN; break;
        case 404: this->response = NOT_FOUND; break;
        case 408: this->response = REQUEST_TIMEOUT; break;
        case 413: this->response = REQUEST_ENTITY_TOO_LARGE; break;
        case 486: this->response = BUSY_HERE; break;
        default: this->response = OTHER; break;
    }
}


SIPType SIPMessage::GetMessageType(const std::string &message)
{
    if (message.find("SIP") == 0)
    {
        SetResponse(message);
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
    if (message.find("MESSAGE") == 0)
        return MESSAGE;
    return UNKNOWN;
}


void    SIPMessage::ParseSIP(const char *sip_buffer)
{
    std::string message(sip_buffer);

    this->type = GetMessageType(message);

    if (this->type == UNKNOWN)
        throw std::runtime_error("Unknown SIP Request.");
    
    this->via = GetHeader(message, "Via: ");
    if (this->via.empty())
        throw std::runtime_error("Missing SIP 'Via' header.");

    this->from_tag = GetHeader(message, "From: ");
    if (this->from_tag.empty())
        throw std::runtime_error("Missing SIP 'From' header");
    else
    {
        if (this->from_tag.find(";") != std::string::npos)
            this->from = this->from_tag.substr(0, this->from_tag.find(";"));
        else
            this->from = this->from_tag;
    }

    this->to = GetHeader(message, "To: ");
    if (this->to.empty())
        throw std::runtime_error("Missing SIP 'To' header.");

    this->cseq = GetHeader(message, "CSeq: ");
    this->call_id = GetHeader(message, "Call-ID: ");
    this->contact = GetHeader(message, "Contact: ");

    std::string expires = GetHeader(message, "Expires: ");
    if (!expires.empty())
        this->expires = std::stoi(expires);

    std::string content_length = GetHeader(message, "Content-Length: ");
    if (!content_length.empty())
        this->content_length = std::stoi(content_length);

    this->body = GetBody(message);
}
