#pragma once

#include <sys/socket.h>
#include <cstring>
#include <sys/poll.h>

const int kClientNameSize = 32;

class Client
{
public:
    Client(const char * name);
    ~Client();
    const char * GetName();
private:
    char _name[kClientNameSize + 1];
};
