# include "SIP.hpp"

/* Constructor y destructor */
/**
 * @brief Constructor de la clase SIP.
 *
 * @param clients Puntero a la lista de clientes registrados.
 * @param client_count Puntero al contador de clientes.
 * @param _addr Dirección del cliente remitente.
 * @param socket Socket configurado para comunicaciones SIP.
 * @param msg Instancia del mensaje SIP recibido.
 */
SIP::SIP(client_t *clients, short *client_count, const struct sockaddr_in &_addr,
    const Socket &socket, SIPMessage &msg) 
    : _clients(clients), _client_count(client_count), _addr(_addr),
    _socket(socket), _msg(msg) {}


SIP::~SIP() {}


/* Metodo publico */
/**
 * @brief Maneja el mensaje SIP recibido y determina la acción a realizar.
 *        Llamando a la funcion auxiliar según el tipo de mensaje.
 *
 * @throws std::runtime_error si el tipo de mensaje no es reconocido.
 * @return void
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
/**
 * @brief Crea y envía la respuesta SIP al cliente indicado.
 *        Llama a la función SendSIPMessage() para enviar la respuesta.
 *
 * @param code Código de respuesta SIP (ej. 200, 404).
 * @param client Puntero al cliente al que se enviará la respuesta.
 * @note  client puede ser NULL si se desea enviar la respuesta al remitente original.
 * @return void
 */
void    SIP::SendResponse(const short &code, client_t *client)
{
    std::string phrase = GetSIPReasonPhrase(code);
    std::ostringstream  response;
    response << "SIP/2.0 " << code << " " << phrase << "\r\n";
    response << "Via: " << this->_msg.via << "\r\n";
    response << "From: " << this->_msg.from_tag << "\r\n";
    response << "To: " << this->_msg.to_tag << "\r\n";
    response << "Call-ID: " << this->_msg.call_id << "\r\n";
    response << "CSeq: " << this->_msg.cseq << "\r\n";

    if ((code == 180 || code == 200) && client != NULL)
    {
        char socket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->_socket.socket_address.sin_addr), socket_ip, INET_ADDRSTRLEN);
        response << "Contact: <sip:" << socket_ip << ":"
                  << ntohs(this->_socket.socket_address.sin_port) << ">\r\n";
    }
    else if (!this->_msg.contact.empty())
        response << "Contact: " << this->_msg.contact << "\r\n";

    if (this->_msg.expires != 0)
    	response << "Expires: " << this->_msg.expires << "\r\n";

    if (!this->_msg.body.empty() && code != 100)
    {
        response << "Content-Length: " << this->_msg.content_length << "\r\n";
        response << "Content-Type: " << this->_msg.content_type << "\r\n";
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


/**
 * @brief Crea y envia la solicitud SIP al cliente destinatario, o 404 si no se encuentra.
 *        Llama a la funcion SendSIPMessage() o SendResponse() según corresponda.
 *
 * @param method Método SIP a enviar (ej. INVITE, ACK).
 * @note  En el método INVITE, se añade cabecera "Via" con la dirección del servidor.
 * @throws std::runtime_error si el destinatario no se encuentra en la lista de clientes.
 * @return void
 */
void    SIP::SendRequest(const std::string &method)
{
    client_t *client = FindClient(this->_clients, this->_msg.to.c_str(), *this->_client_count);
    if (client == NULL)
    {
        SendResponse(404, NULL); // Not Found
        throw std::runtime_error("404 Receiver Not Found.");
    }

    
    char no_brackets[MAX_SIP_URI];
    RemoveBrackets(no_brackets, client->uri);
    
    std::ostringstream  request;
    request << method << " " << no_brackets << " SIP/2.0\r\n";
    
    if (method == "INVITE")
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

    if (method != "ACK")
    {
        char socket_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->_socket.socket_address.sin_addr), socket_ip, INET_ADDRSTRLEN);
        request << "Contact: <sip:" << socket_ip << ":"
                      << ntohs(this->_socket.socket_address.sin_port) << ">\r\n";
    }

    if (this->_msg.expires != 0)
        request << "Expires: " << this->_msg.expires << "\r\n";

    if (!this->_msg.content_type.empty())
        request << "Content-Type: " << this->_msg.content_type << "\r\n";
    request << "Content-Length: " << this->_msg.content_length << "\r\n";
    request << "\r\n";

    if (!this->_msg.body.empty())
        request << this->_msg.body;

    SendSIPMessage(request.str(), client->addr, client->uri, false);
}


/* Metodos privados auxiliares */
/**
 * @brief Obtiene la "reason phrase" SIP correspondiente al código de respuesta.
 *
 * @param code Código de respuesta SIP (ej. 200, 404).
 * @note si el codigo no es reconocido, devuelve "Unknown".
 * @return std::string La "reason phrase" SIP.
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

/**
 * @brief Envía un mensaje SIP al cliente indicado.
 *
 * @param message Mensaje SIP a enviar.
 * @param addr Dirección del cliente destinatario.
 * @param uri URI del cliente destinatario.
 * @param is_response Indica si el mensaje es una respuesta (true) o una solicitud (false).
 * @throws std::runtime_error si ocurre un error al enviar el mensaje.
 * @return void
 */
void    SIP::SendSIPMessage(const std::string &message, const struct sockaddr_in &addr,
                            const std::string &uri, const bool &is_response)
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
            throw std::runtime_error("Error sending SIP request.");

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        std::cout << GREEN << "Sending request to: " << client_ip << ":" << ntohs(addr.sin_port);
        std::cout << " " << uri << std::endl;
        std::cout << message << RESET << std::endl;
    }
}

/**
 * @brief Verifica si el campo "Contact" del mensaje SIP está vacío.
 *        Si está vacío, lo completa con la dirección y puerto del cliente.
 *
 * @return void
 */
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