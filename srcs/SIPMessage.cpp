# include "SIPMessage.hpp"

/* Funciones auxiliares estaticas */
/**
 * @brief Obtiene el cuerpo del mensaje SIP a partir de la cadena de texto del mensaje.
 *
 * @param message Cadena de texto que representa el mensaje SIP.
 * @note Se espera el formato estándar de un mensaje SIP, el cuerpo se encuentra
 *       después de la ultima línea de encabezados separada por dos saltos de línea (\r\n\r\n).
 * @return Cadena de texto que representa el cuerpo del mensaje o vacio si no se encuentra.
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
    this->expires = 3600;
    this->content_length = 0;
}


SIPMessage::~SIPMessage() {}


/* Metodos publicos */
/**
 * @brief Analiza un mensaje SIP recibido y extrae la información relevante.
 *        Establece el tipo de mensaje, los encabezados y el cuerpo del mensaje.
 *        Se divide en funciones auxiliares para manejar los encabezados específicos.
 *
 * @param sip_buffer Cadena de caracteres que representa el mensaje SIP recibido.
 * @throws std::runtime_error Si el mensaje no tiene un formato válido, faltan encabezados
 *         requeridos o el tipo de mensaje no se puede determinar.
 * @return void
 */
void    SIPMessage::ParseSIP(const char *sip_buffer)
{
    std::istringstream stream(sip_buffer);
    std::string line;
    bool found_via = false;

    if (!std::getline(stream, line, '\r'))
        throw std::runtime_error("Invalid SIP message.");

    stream.get();
    this->type = GetMessageType(line);
    if (this->type == UNKNOWN)
        throw std::runtime_error("Unknown SIP Request.");

    std::string header, value;
    while (std::getline(stream, line, '\r') && !line.empty())
    {
        stream.get();
        size_t pos = line.find(':');
        if (pos == std::string::npos)
            continue;

        header = line.substr(0, pos);
        value = line.substr(pos + 2);

        if (header == "Via")
            ViaCase(&found_via, value);
        else if (header == "From")
            FromCase(value);
        else if (header == "To")
            ToCase(value);
        else
            GetHeaders(header, value);
    }

    if (this->content_length > 0)
        this->body = GetBody(stream.str());
}


/* Metodos privados */
/**
 * @brief Establece el tipo de mensaje SIP basado en la primera línea del mensaje.
 *        Puede ser un mensaje de respuesta o un mensaje de solicitud como REGISTER,
 *        INVITE, ACK, MESSAGE, CANCEL o BYE.
 *
 * @param line Cadena de texto que representa la primera línea del mensaje SIP.
 * @note Se espera que la primera linea del mensaje SIP tenga el formato estándar:
 *       "SIP/2.0 <status_code> <status_text>" o "<method> <uri> SIP/2.0"
 * @return SIPType El tipo de mensaje SIP o UNKNOWN si no se reconoce.
 */
SIPType SIPMessage::GetMessageType(const std::string &line)
{
    if (line.find("SIP") == 0)
    {
        SetResponse(line);
        return RESPONSE;
    }
    if (line.find("REGISTER") == 0)
        return REGISTER;
    if (line.find("INVITE") == 0)
        return INVITE;
    if (line.find("ACK") == 0)
        return ACK;
    if (line.find("MESSAGE") == 0)
        return MESSAGE;
    if (line.find("CANCEL") == 0)
        return CANCEL;
    if (line.find("BYE") == 0)
        return BYE;
    return UNKNOWN;
}


/**
 * @brief Establece el tipo de respuesta encontrado en el mensaje SIP recibido (si aplica).
 *
 * @param line Cadena de texto que representa la primera linea del mensaje SIP.
 * @throws std::runtime_error Si el mensaje no tiene un formato válido y no se puede
 *         extraer un código de estado.
 * @note Se espera que la primera linea del mensaje SIP tenga el formato estándar:
 *       "SIP/2.0 <status_code> <status_text>"
 * @return void
 */
void    SIPMessage::SetResponse(const std::string &line)
{
    size_t  start = line.find(' ');
    if (start == std::string::npos)
        throw std::runtime_error("Invalid SIP line format.");

    size_t end = line.find(' ', start + 1);
    if (end == std::string::npos)
        throw std::runtime_error("Invalid SIP line format.");

    std::string status_code = line.substr(start + 1, end - start - 1);

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


/**
 * @brief Maneja el encabezado "Via" del mensaje SIP.
 *
 * @param found_via Puntero a un booleano que indica si se ha encontrado el encabezado "Via".
 * @param value Cadena de texto que representa el valor del encabezado "Via".
 * @throws std::runtime_error Si el encabezado "Via" está vacío.
 * @return void
 */
void    SIPMessage::ViaCase(bool *found_via, const std::string &value)
{
    if (*found_via)
        this->via += "\r\n" + value;
    else
    {
        this->via = value;
        *found_via = true;
    }
    if (this->via.empty())
        throw std::runtime_error("Missing SIP 'Via' header.");
}


/**
 * @brief Maneja el encabezado "From" del mensaje SIP.
 *
 * @param value Cadena de texto que representa el valor del encabezado "From".
 * @throws std::runtime_error Si el encabezado "From" está vacío.
 * @return void
 */
void    SIPMessage::FromCase(const std::string &value)
{
    this->from_tag = value;
    if (this->from_tag.empty())
        throw std::runtime_error("Missing SIP 'From' header.");
    else
    {
        if (this->from_tag.find(";") != std::string::npos)
            this->from = this->from_tag.substr(0, this->from_tag.find(";"));
        else
            this->from = this->from_tag;
    }
}


/**
 * @brief Maneja el encabezado "To" del mensaje SIP.
 *
 * @param value Cadena de texto que representa el valor del encabezado "To".
 * @throws std::runtime_error Si el encabezado "To" está vacío.
 * @return void
 */
void    SIPMessage::ToCase(const std::string &value)
{
    this->to_tag = value;
    if (this->to_tag.empty())
        throw std::runtime_error("Missing SIP 'To' header.");
    else
    {
        if (this->to_tag.find(";") != std::string::npos)
            this->to = this->to_tag.substr(0, this->to_tag.find(";"));
        else
            this->to = this->to_tag;
    }
}


/**
 * @brief Maneja los encabezados auxiliares del mensaje SIP.
 *
 * @param header Cadena de texto que representa el nombre del encabezado.
 * @param value Cadena de texto que representa el valor del encabezado.
 * @throws std::runtime_error Si el encabezado CSeq o Call-ID está vacío.
 * @return void
 */
void    SIPMessage::GetHeaders(const std::string &header, const std::string &value)
{
    if (header == "CSeq")
    {
        this->cseq = value;
        if (this->cseq.empty())
            throw std::runtime_error("Missing SIP 'CSeq' header.");
    }
    else if (header == "Call-ID")
    {
        this->call_id = value;
        if (this->call_id.empty())
            throw std::runtime_error("Missing SIP 'Call-ID' header.");
    }
    else if (header == "Contact")
        this->contact = value;
    else if (header == "Content-Type")
        this->content_type = value;
    else if (header == "Expires")
        this->expires = std::stoi(value);
    else if (header == "Content-Length")
        this->content_length = std::stoi(value);
}