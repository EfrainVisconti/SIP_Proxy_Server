# include "../incs/MCXServer.hpp"

/**
 * @brief Verifica si un buffer está vacío.
 *
 * @param buffer Cadena de caracteres a verificar.
 * @return true si el buffer está vacío, false en caso contrario.
 */
bool IsEmptyBuffer(const char *buffer)
{
    if (buffer == NULL)
        return true;

    if (buffer[0] == '\0')
        return true;

    for (int i = 0; buffer[i]; i++)
    {
        if (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r')
            return false;
    }
    return true;
}


/**
 * @brief Elimina los corchetes angulares de una URI SIP.
 *
 * @param aux Cadena auxiliar donde se almacenará la URI sin corchetes.
 * @param uri Cadena de entrada que contiene la URI SIP.
 * @return void
 */
void    RemoveBrackets(char *aux, const char *uri)
{
    if (aux == NULL || uri == NULL)
        return;

    size_t len = strlen(uri);
    if (len > 2 && uri[0] == '<' && uri[len - 1] == '>')
    {
        strncpy(aux, uri + 1, len - 2);
        aux[len - 2] = '\0';
    }
    else
        strcpy(aux, uri);
}