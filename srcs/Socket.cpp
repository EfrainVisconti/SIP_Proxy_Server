# include "Socket.hpp"

/* Constructor, destructor y forma canonica ortodoxa */
Socket::Socket(const char *host, const uint16_t &port) : host(inet_addr(host)), port(port)
{
	this->fd = 0;
    CreateSocket();
}


Socket::~Socket() {}


Socket::Socket(const Socket &other)
{
    *this = other;
}


Socket &Socket::operator=(const Socket &other)
{
    if (this != &other)
    {
        this->host = other.host;
        this->port = other.port;
        this->socket_address = other.socket_address;
        this->fd = other.fd;
    }
    return *this;
}


/* Metodos privados */
/*
	Implementa la secuencia socket(), bind().
	* fd: identifica al socket que quedará a la espera de recibir.
	* socket_address: identifica la dirección y puerto del servidor.

	* setsockopt() se usa para evitar el error "Address already in use".
	* fcntl() configura el socket en modo no bloqueante (O_NONBLOCK).

	NOTA: lanza std::runtime_error() explícita en caso de error.
*/
void    Socket::CreateSocket()
{
	int opt = 1;

    if ((this->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        throw std::runtime_error("socket() error. Stopping execution...");

    if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(this->fd);
        throw std::runtime_error("setsockopt() error. Stopping execution...");
    }

    int flags = fcntl(this->fd, F_GETFL, 0);
    if (flags == -1 || fcntl(this->fd, F_SETFL, flags | O_NONBLOCK) == -1) 
    {
        close(this->fd);
        throw std::runtime_error("fcntl() error. Stopping execution...");
    }

    SetSockaddr_in();
    if (bind(this->fd, (struct sockaddr *) &(this->socket_address), sizeof(this->socket_address)) == -1)
    {
		close(this->fd);
		throw std::runtime_error("bind() error. Stopping execution...");
    }
}


/*
	Configura la estructura sockaddr_in, en la cual se identifica la
	familia de la dirección, la dirección IPv4 y el puerto del socket,
	es almacenada en socket_address). 
*/
void    Socket::SetSockaddr_in()
{
	memset(&(this->socket_address), 0, sizeof(this->socket_address));
	this->socket_address.sin_family = AF_INET;
    this->socket_address.sin_addr.s_addr = this->host;
    this->socket_address.sin_port = htons(this->port);
}