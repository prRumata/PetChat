#pragma once

#include <cwchar>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <iostream>

const int kMaxMessageLength = 4096;

class Client
{
public:
    Client(char *ip, int port);
    ~Client();
    void start();
private:
    void init();
    void waiting();
    void get_message();
    void send_message();

    pollfd _file_descriptors[2];
    sockaddr_in _address;
    socklen_t _address_size;
    int _socket;
    int _port;
    char _ip[16];
    char _message[kMaxMessageLength + 1];
};
