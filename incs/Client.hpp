#ifndef CLIENT_HPP
# define CLIENT_HPP

# define MAX_SIP_URI 256
# define MAX_SIP_CLIENTS 100

enum ClientStatus
{
    RECENTLY_REGISTERED,
    RESPONDING_TO_INVITE, // Secuencia 100 Trying, 180 Ringing, 200 OK
    WAITING_200, // Esperando 200 OK tras enviar INVITE inicial
    SENDING_ACK, // Enviando ACK tras recibir 200 OK
    CONNECTED, // Llamada establecida
    SENDED_MESSAGE
};

typedef struct client_s
{
    struct sockaddr_in  addr;
    char         uri[MAX_SIP_URI];
    ClientStatus status;
} client_t;

#endif