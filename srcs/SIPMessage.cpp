# include "SIPMessage.hpp"

/* Funciones auxiliares estaticas */
/*

*/
static std::string  GetHeaderVia(const std::string &message, size_t &start) 
{
    std::string ret;
    bool is_first = true;

    while (message.find("Via :", start) == start)
    {
        size_t end = message.find("\r\n", start);
        if (end == std::string::npos)
            return "";

        if (is_first == false)
            ret += "\r\n";
        else
            is_first = false;

        ret += message.substr(start, end - start);
        start = end + 2;
    }

    if (ret.empty())
        return "";

    return ret;
}


/*

*/
static std::string  GetHeader(const std::string &message, const std::string &header)
{
    size_t start = message.find(header);
    if (start == std::string::npos)
        return "";

    if (header == "Via :")
        return GetHeaderVia(message, start);

    start += header.length();
    size_t end = message.find("\r\n", start);
    if (end == std::string::npos)
        throw std::runtime_error("Invalid SIP message format.");

    std::string ret = message.substr(start, end - start);
    if (ret.empty())
        return "";

    return ret;
}


/*

*/
static std::string  GetBody(const std::string &message)
{
    size_t start = message.find("\r\n\r\n");
    if (start == std::string::npos)
        return "";

    start += 4;
    std::string ret = message.substr(start);
    if (ret.empty())
        return "";

    return ret;
}


/* Constructor y destructor */
SIPMessage::SIPMessage ()
{
    this->type = UNKNOWN;
    this->response = OTHER;
    this->expires = 0;
    this->content_length = 0;
}


SIPMessage::~SIPMessage() {}


/* Metodos publicos */
/*

*/
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
        throw std::runtime_error("Missing SIP 'From' header.");
    else
    {
        if (this->from_tag.find(";") != std::string::npos)
            this->from = this->from_tag.substr(0, this->from_tag.find(";"));
        else
            this->from = this->from_tag;
    }

    this->to_tag = GetHeader(message, "To: ");
    if (this->to_tag.empty())
        throw std::runtime_error("Missing SIP 'To' header.");
    else
    {
        if (this->to_tag.find(";") != std::string::npos)
            this->to = this->to_tag.substr(0, this->to_tag.find(";"));
        else
            this->to = this->to_tag;
    }
    

    this->cseq = GetHeader(message, "CSeq: ");
    this->call_id = GetHeader(message, "Call-ID: ");
    this->contact = GetHeader(message, "Contact: ");
    this->content_type = GetHeader(message, "Content-Type: ");

    std::string str_expires = GetHeader(message, "Expires: ");
    if (!str_expires.empty())
        this->expires = std::stoi(str_expires);

    std::string str_content_length = GetHeader(message, "Content-Length: ");
    if (!str_content_length.empty())
        this->content_length = std::stoi(str_content_length);

    this->body = GetBody(message);
}


/* Metodos privados */
/*

*/
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
    if (message.find("ACK") == 0)
        return ACK;
    if (message.find("MESSAGE") == 0)
        return MESSAGE;
    if (message.find("CANCEL") == 0)
        return CANCEL;
    if (message.find("BYE") == 0)
        return BYE;
    return UNKNOWN;
}


/*

*/
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