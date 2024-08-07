#include "Server.h"
#include <unistd.h>

Server::Server(int port)
{
    _port = port;
}

Server::~Server()
{
    for (int i = 0; i < _file_descriptors_size; ++i)
    {
        close(_file_descriptors[i].fd);
    }
}

void Server::start()
{
    init();

    while(true)
    {
        waiting();
        new_connections();
        get_message();
    }
}

void Server::init()
{
    _address_size = sizeof(sockaddr_in);
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        std::cerr << "Error: Failed to create socket.\n";
        std::cerr << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    _address.sin_family = AF_INET;
    _address.sin_port = htons(_port);
    _address.sin_addr.s_addr = INADDR_ANY;

    if (bind(_socket, (sockaddr*)&_address, _address_size) == -1)
    {
        std::cerr << "Error: Failed to bind socket.\n";
        std::cerr << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(_socket, 5) == -1)
    {
        std::cerr << "Error: Failed to listen socket.\n";
        std::cerr << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    _file_descriptors_size = 1;
    _file_descriptors[0].fd = _socket;
    _file_descriptors[0].events = POLLIN;

    for (int i = 1; i <= kMaxClients; ++i)
    {
        _file_descriptors[i].fd = -1;
    }

    std::cout << "Initialization successful.\n"
              << "Waiting for connections...\n";
}

void Server::waiting()
{
    int poll_return;

    poll_return = poll(_file_descriptors, _file_descriptors_size, -1);

    if (poll_return == -1)
    {
        std::cerr << "Error: Poll.\n";
        std::cerr << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::new_connections()
{
    int client_socket;
    if (_file_descriptors[0].revents & POLLIN)
    {
        client_socket = accept(_socket, (sockaddr*)&_address, &_address_size);
        if (client_socket == -1)
        {
            std::cerr << "Error: Failed to accept\n";
            std::cerr << std::strerror(errno) << std::endl;
            return;
        }

        if (_file_descriptors_size > kMaxClients)
        {
            std::cerr << "Error: Failed to accept. Server is full\n";
            close(client_socket);
            return;
        }

        _file_descriptors[_file_descriptors_size].fd = client_socket;
        _file_descriptors[_file_descriptors_size].events = POLLIN;
        ++_file_descriptors_size;
    }
}

void Server::get_message()
{
    ssize_t message_length;
    for (int i = 1; i < _file_descriptors_size; ++i)
    {
        if (_file_descriptors[i].revents & POLLIN)
        {
            message_length = read(_file_descriptors[i].fd, _message, kMaxMessageLength);
            if (message_length == -1)
            {
                std::cerr << "ERROR: Failed to reading from socket.\n";
                std::cerr << std::strerror(errno) << std::endl;
            }
            else if (message_length == 0)
            {
                std::cout << "Client disconnected.\n";
                close(_file_descriptors[i].fd);
                _file_descriptors[i] = _file_descriptors[_file_descriptors_size - 1];
                --_file_descriptors_size;
                --i;
            }
            else
            {
                if (_message[message_length - 1] == '\n')
                    --message_length;
                if (message_length == 0) continue;
                _message[message_length] = '\0';
                std::cout << "Message: " << _message << std::endl;
                send_message(_message, message_length + 1);
            }
        }
    }
}

void Server::send_message(char * message, size_t message_lenght)
{
    for (int i = 1; i < _file_descriptors_size; ++i)
    {
        write(_file_descriptors[i].fd, message, message_lenght);
    }
}
