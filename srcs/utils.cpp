# include "../incs/MCXServer.hpp"

/*

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


/*

*/
void    RemoveBrackets(char *aux, const char *uri)
{
    size_t len = strlen(uri);
    if (uri[0] == '<' && uri[len - 1] == '>')
    {
        strncpy(aux, uri + 1, len - 2);
        aux[len - 2] = '\0';
    }
    else
        strcpy(aux, uri);
}