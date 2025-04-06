#include "../incs/MCXServer.hpp"

/**
 * @brief Formatea la URI del cliente agregando los caracteres '<' y '>'.
 *        Si la URI ya tiene estos caracteres, se hace una copia.
 *
 * @param input URI del cliente.
 * @param output Buffer donde se almacenará la URI formateada.
 * @note  Se utiliza strncpy() para evitar desbordamientos de buffer.
 * @return void
 */
static void FindClientAux(const char *input, char *output)
{
   size_t len = strlen(input);
   if (len > 0 && input[0] != '<' && input[len - 1] != '>')
   {
       output[0] = '<';
       strncpy(output + 1, input, len);
       output[len + 1] = '>';
       output[len + 2] = '\0';
   }
   else
       strcpy(output, input);
}

/**
 * @brief Busca un cliente en la lista de clientes SIP registrados.
 *        Llama a la función auxiliar FindClientAux() para formatear la URI.
 * 
 * @param clients Lista de clientes.
 * @param uri URI del cliente a buscar.
 * @param client_count Cantidad de clientes registrados.
 * @return Puntero al cliente encontrado, o NULL si no se encuentra.
 */
client_t    *FindClient(client_t *clients, const char *uri, short client_count)
{
    if (clients == NULL || uri == NULL)
    {
        std::cout << "Error: Argumentos invalidos en FindClient()." << std::endl;
        return NULL;
    }

    char    uri_format[MAX_SIP_URI + 2];
    FindClientAux(uri, uri_format);

    for (int i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i].uri, uri_format) == 0)
            return &clients[i];
    }
    return NULL;
}


/**
 * @brief Agrega un cliente a la lista de clientes SIP registrados.
 * 
 * @param clients Lista de clientes.
 * @param uri URI del cliente a agregar.
 * @param client_addr Dirección del cliente.
 * @param client_count Cantidad de clientes registrados.
 * @param status Estado del cliente (RECENTLY_REGISTERED).
 * 
 * @note  Se verifica que la URI no exceda el tamaño máximo permitido (MAX_SIP_URI).
 * @note  Se verifica que la lista de clientes no esté llena (MAX_SIP_CLIENTS).
 * @return true si el cliente fue agregado, false si hubo un error.
 */
bool AddClient(client_t *clients, const char *uri, const struct sockaddr_in &client_addr,
                short *client_count, ClientStatus status)
{
    if (clients == NULL || uri == NULL || client_count == NULL)
    {
        std::cout << "Error: Argumentos invalidos en AddClient()." << std::endl;
        return false;
    }

    if (*client_count >= MAX_SIP_CLIENTS)
        return false;

        
    if (strlen(uri) >= MAX_SIP_URI)
    {
        std::cout << "Error: La URI del cliente es demasiado larga." << std::endl;
        return false;
    }
        
    clients[*client_count].addr = client_addr;
    clients[*client_count].status = status;
    strncpy(clients[*client_count].uri, uri, MAX_SIP_URI - 1);
    clients[*client_count].uri[MAX_SIP_URI - 1] = '\0';

    (*client_count)++;
    return true;
}


/**
 * @brief Elimina un cliente de la lista de clientes SIP registrados.
 * 
 * @param clients Lista de clientes.
 * @param uri URI del cliente a eliminar.
 * @param client_count Cantidad de clientes registrados.
 * @return true si el cliente fue eliminado, false si no se encontró.
 */
bool    RemoveClient(client_t *clients, const char *uri, short *client_count)
{
    for (short i = 0; i < *client_count; i++)
    {
        if (strcmp(clients[i].uri, uri) == 0)
        {

            for (int j = i; j < *client_count - 1; j++)
                clients[j] = clients[j + 1];

            (*client_count)--;
            return true;
        }
    }
    return false;
}


/**
 * @brief Imprime la lista de clientes SIP registrados.
 * 
 * @param clients Lista de clientes.
 * @param client_count Cantidad de clientes registrados.
 * @note Si no hay clientes registrados, se imprime un mensaje indicando que no hay clientes.
 * @return void
 */
void    PrintClients(client_t *clients, short client_count)
{
    if (client_count == 0)
    {
        std::cout << "No hay clientes SIP registrados." << std::endl;
        return;
    }

    std::cout << "Clientes SIP registrados:" << std::endl;
    for (short i = 0; i < client_count; i++)
        std::cout << "Cliente " << i + 1 << ": " << clients[i].uri << std::endl;
}