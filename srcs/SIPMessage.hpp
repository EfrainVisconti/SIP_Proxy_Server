#ifndef SIPMESSAGE_HPP
# define SIPMESSAGE_HPP

# include "../incs/MCXServer.hpp"

enum SIPType
{
    RESPONSE,
    REGISTER,
    INVITE,
    BYE,
    CANCEL,
    ACK,
    MESSAGE,
    UNKNOWN
};

enum SIPResponse
{
	OK,
	TRYING,
	RINGING,
	BAD_REQUEST,
	UNAUTHORIZED,
	FORBIDDEN,
	NOT_FOUND,
	REQUEST_TIMEOUT,
	REQUEST_ENTITY_TOO_LARGE,
	BUSY_HERE,
    OTHER
};

/**
 * @class SIPMessage
 * @brief Clase que representa un mensaje SIP.
 * 
 * Esta clase encapsula un mensaje SIP recibido de un cliente y proporciona métodos
 * para analizar y gestionar el mensaje. Incluye información como el tipo de mensaje,
 * la respuesta SIP (si aplica), los encabezados y el cuerpo.
 */
class SIPMessage
{
    public:
        SIPType		type;
        SIPResponse	response;
        std::string	via;
        std::string	to;
        std::string	to_tag;
        std::string	from;
        std::string	from_tag;
        std::string	call_id;
        std::string	cseq;
        std::string	contact;
        int         expires;
        std::string	content_type;
        int         content_length;
        std::string	body;

        SIPMessage();
	    ~SIPMessage();
	    void	ParseSIP(const char *sip_buffer);

    private:
        SIPType GetMessageType(const std::string &message);
        void    SetResponse(const std::string &message);
        void    ViaCase(bool *found_via, const std::string &value);
        void    FromCase(const std::string &value);
        void    ToCase(const std::string &value);
        void    GetHeaders(const std::string &header, const std::string &value);
};

#endif