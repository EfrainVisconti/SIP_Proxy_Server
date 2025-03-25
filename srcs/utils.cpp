#include "../incs/MCXServer.hpp"

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