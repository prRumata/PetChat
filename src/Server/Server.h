#pragma once

#include "Client.h"
#include <cstdlib>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

const int kMaxClients = 2;
const int kMaxMessageLength = 4096;

class Server
{
public:
    Server(int port);
    ~Server();
    void Start();
private:
    void Init();
    void Waiting();
    void NewConnections();
    void GetMessage();
    const void SendMessage(const Client & client, const char * message, size_t message_lenght) const;

    Client _clients[kMaxClients];
    size_t _clients_size;
    pollfd _file_descriptors[kMaxClients + 1];
    nfds_t _file_descriptors_size;
    sockaddr_in _address;
    socklen_t _address_size;
    char _message[kMaxMessageLength + kClientNameSize + 4];
    int _socket;
    int _port;
};
