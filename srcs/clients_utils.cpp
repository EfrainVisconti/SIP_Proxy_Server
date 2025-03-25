#include "../incs/MCXServer.hpp"

bool AddClient(clients_t *clients, const char *info, const struct sockaddr_in &client_addr, short *client_count, ClientStatus status)
{
    if (*client_count >= MAX_SIP_CLIENTS)
        return false;

    clients[*client_count].addr = client_addr;
    clients[*client_count].status = status;

    if (strlen(info) >= sizeof(clients[*client_count].info))
    {
        printf("Error: La información del cliente es demasiado larga.\n");
        return false;
    }

    strncpy(clients[*client_count].info, info, sizeof(clients[*client_count].info) - 1);
    clients[*client_count].info[sizeof(clients[*client_count].info) - 1] = '\0';

    (*client_count)++;
    return true;
}


bool    RemoveClient(clients_t *clients, const char *info, short *client_count)
{
    for (short i = 0; i < *client_count; i++)
    {
        if (strcmp(clients[i].info, info) == 0)
        {

            for (int j = i; j < *client_count - 1; j++)
                clients[j] = clients[j + 1];

            (*client_count)--;
            return true;
        }
    }
    return false;
}


clients_t*  FindClient(clients_t *clients, const char *info, short client_count)
{
    for (int i = 0; i < client_count; i++)
    {
        if (strcmp(clients[i].info, info) == 0)
            return &clients[i];
    }
    return NULL;
}


void    PrintClients(clients_t *clients, short client_count)
{
    if (client_count == 0)
    {
        printf("No hay clientes SIP registrados.\n");
        return;
    }

    printf("Lista de clientes SIP:\n");
    for (short i = 0; i < client_count; i++)
        printf("Cliente %d: %s\n", i + 1, clients[i].info);
}
