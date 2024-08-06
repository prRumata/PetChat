#pragma once

#include <cstdlib>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

const int kMaxClients = 128;
const int kMaxMessageLength = 4096;

class Server
{
public:
    Server(int port);
    ~Server();
    void start();
private:
    void init();
    void waiting();
    void new_connections();
    void get_message();
    void send_message(char * message, size_t message_lenght);

    pollfd _file_descriptors[kMaxClients + 1];
    nfds_t _file_descriptors_size;
    sockaddr_in _address;
    socklen_t _address_size;
    char _message[kMaxMessageLength + 1];
    int _socket;
    int _port;

};
