#pragma once

#include <sys/socket.h>
#include <string>
#include <sys/poll.h>

const int kClientNameSize = 32;

class Client
{
public:
    Client();
    ~Client();
private:
    std::string _name;
};