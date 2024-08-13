#include "Client.h"
#include <iostream>

Client::Client()
{
    _name_size = 0;
}

Client::~Client()
{

}

const char * Client::GetName() const
{
    return _name;
}

void Client::SetName(const char * name)
{
    std::strncpy(_name, name, kClientNameSize);
    _name_size = std::strlen(_name);
    _name[_name_size] = '\0';
}

void Client::DeleteUser()
{
    _name_size = 0;
    _name[0] = '\0';
}

size_t Client::GetNameSize() const
{
    return _name_size;
}

bool Client::IsReady() const
{
    return !(_name_size == 0);
}