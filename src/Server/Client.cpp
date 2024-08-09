#include "Client.h"

Client::Client(const char * name)
{
    std::strncpy(_name, name, kClientNameSize);
    _name[kClientNameSize] = '\0';
}

const char * Client::GetName()
{
    return _name;
}
