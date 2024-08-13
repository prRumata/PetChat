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

void Server::Start()
{
    Init();

    while(true)
    {
        Waiting();
        NewConnections();
        GetMessage();
    }
}

void Server::Init()
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

    _clients_size = 0;

    for (int i = 1; i <= kMaxClients; ++i)
    {
        _file_descriptors[i].fd = -1;
    }

    std::cout << "Initialization successful.\n"
              << "Waiting for connections...\n";
}

void Server::Waiting()
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

void Server::NewConnections()
{
    int client_socket;
    if (_file_descriptors[0].revents & POLLIN)
    {
        client_socket = accept(_socket, (sockaddr*)&_address, &_address_size);
        if (client_socket == -1)
        {
            std::cerr << "Error: Failed to accept.\n";
            std::cerr << std::strerror(errno) << std::endl;
            return;
        }

        if (_file_descriptors_size > kMaxClients)
        {
            write(client_socket, "Failed to accept. Server is full.", 41);
            std::cerr << "Error: Failed to accept. Server is full.\n";
            close(client_socket);
            return;
        }

        _file_descriptors[_file_descriptors_size].fd = client_socket;
        _file_descriptors[_file_descriptors_size].events = POLLIN;
        ++_file_descriptors_size;

        ++_clients_size;
    }
}

void Server::GetMessage()
{
    
    for (int i = 0; i < _clients_size; ++i)
    {
        if (_file_descriptors[i + 1].revents & POLLIN)
        {
            if (_clients[i].IsReady())
            {
                ssize_t message_length;
                strcpy(_message, _clients[i].GetName());
                strcat(_message, ": ");
                message_length = read(_file_descriptors[i + 1].fd, _message + _clients[i].GetNameSize() + 2, kMaxMessageLength);
                if (message_length == -1)
                {
                    std::cerr << "Error: Failed to reading from socket.\n";
                    std::cerr << std::strerror(errno) << std::endl;
                }
                else if (message_length == 0)
                {
                    std::cout << "Client disconnected.\n";
                    close(_file_descriptors[i + 1].fd);
                    _file_descriptors[i + 1] = _file_descriptors[_file_descriptors_size - 1];
                    _file_descriptors[_file_descriptors_size - 1].fd = -1;
                    --_file_descriptors_size;

                    _clients[i] = _clients[_clients_size - 1];
                    _clients[_clients_size - 1].DeleteUser();
                    --_clients_size;

                    --i;
                }
                else
                {
                    message_length += _clients[i].GetNameSize() + 2;
                    if (_message[message_length - 1] != '\n')
                    {
                        _message[message_length] = '\n';
                        ++message_length;
                    }    
                    if (message_length == 0) continue;
                    _message[message_length] = '\0';
                    std::cout << _message;
                    SendMessage(_clients[i], _message, message_length + 1);
                }
            }

            else
            {
                ssize_t name_length;
                name_length = read(_file_descriptors[i + 1].fd, _message, kClientNameSize);

                if (name_length <= 0 || _message[0] == '\n')
                {
                    std::cerr << "Error: Failed to reading username from socket.\n";
                    std::cerr << std::strerror(errno) << std::endl;
                    close(_file_descriptors[i + 1].fd);

                    _file_descriptors[i + 1] = _file_descriptors[_file_descriptors_size - 1];
                    _file_descriptors[_file_descriptors_size - 1].fd = -1;
                    --_file_descriptors_size;
                    
                    _clients[i] = _clients[_clients_size - 1];
                    _clients[_clients_size - 1].DeleteUser();
                    --_clients_size;

                    --i;
                }

                if (_message[name_length - 1] == '\n')
                    --name_length;
                _message[name_length] = '\0';
                _clients[i].SetName(_message);

                std::cout << _clients[i].GetName() << " connected!" << std::endl;
            }
        }
    }
}

const void Server::SendMessage(const Client & client, const char * message, size_t message_lenght) const
{
    for (int i = 0; i < _clients_size; ++i)
    {
        if (_clients[i].IsReady()) {
            write(_file_descriptors[i + 1].fd, message, message_lenght);
        }
    }
}
