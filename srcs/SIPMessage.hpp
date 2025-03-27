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
        int         content_length;
        std::string	body;

        SIPMessage();
	    ~SIPMessage();
	    void	ParseSIP(const char *sip_buffer);

    private:
        SIPType GetMessageType(const std::string &message);
        void    SetResponse(const std::string &message);
};

#endif