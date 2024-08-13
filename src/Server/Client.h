#pragma once

#include <sys/socket.h>
#include <cstring>
#include <sys/poll.h>

const int kClientNameSize = 16;

class Client
{
public:
    Client();
    ~Client();
    const char * GetName() const;
    size_t GetNameSize() const;
    void SetName(const char * name);
    bool IsReady() const;
    void DeleteUser();
private:
    char _name[kClientNameSize + 1];
    size_t _name_size;
};
