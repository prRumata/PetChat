#include "Client.h"
#include <cstdlib>
#include <unistd.h>

Client::Client(char *ip, int port)
{
    std::strcpy(_ip, ip);
    _port = port;
}

Client::~Client()
{
    close(_socket);
}

void Client::start()
{
    init();

    while(true)
    {
        waiting();
        get_message();
        send_message();
    }
}

void Client::init()
{
    _address_size = sizeof(sockaddr_in);
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        std::cerr << "Error: Failed to create socket.\n";
        exit(EXIT_FAILURE);
    }

    _address.sin_family = AF_INET;
    _address.sin_port = htons(_port);
    if (inet_pton(AF_INET, _ip, &_address.sin_addr) < 1)
    {
        std::cerr << "Error: Incorect IP address.\n";
        exit(EXIT_FAILURE);
    }

    if (connect(_socket, (sockaddr *)&_address, _address_size) == -1)
    {
        std::cerr << "Error: Faild to connect to server.\n";
        exit(EXIT_FAILURE);
    }

    _file_descriptors[0].fd = _socket;
    _file_descriptors[0].events = POLLIN;

    _file_descriptors[1].fd = STDIN_FILENO;
    _file_descriptors[1].events = POLLIN;
}

void Client::waiting()
{
    int poll_return;
    poll_return = poll(_file_descriptors, 2, -1);

    if (poll_return == -1)
    {
        std::cerr << "Error: Poll.\n";
        exit(EXIT_FAILURE);
    }
}


void Client::get_message()
{
    ssize_t message_size;
    if (_file_descriptors[0].revents & POLLIN)
    {
        message_size = read(_file_descriptors[0].fd, _message, kMaxMessageLength);
        if (message_size == -1)
        {
            std::cerr << "ERROR: Failed to reading from server.\n";
        }
        else if (message_size == 0)
        {
            std::cout << "Server disconnected.\n";
            close(_file_descriptors[0].fd);
            exit(EXIT_FAILURE);
        }
        else {
            std::cout << _message;
        }
    }
}

void Client::send_message()
{
    ssize_t message_lenght;
    if (_file_descriptors[1].revents & POLLIN)
    {
        message_lenght = read(_file_descriptors[1].fd, _message, kMaxMessageLength);
        if (message_lenght > 0)
        {
            _message[kMaxMessageLength] = '\0';
            write(_file_descriptors[0].fd, _message, message_lenght);
        }
    }
}
