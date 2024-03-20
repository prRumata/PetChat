#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits>


int Start(int& client_socket, sockaddr_in& server_addr) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    uint16_t port;
    char ip[16];
    std::string name;

    std::cout << "Name: ";
    std::getline(std::cin, name);
    std::cout << "Port: ";
    std::cin >> port;
    std::cout << "IP: ";
    std::cin >> ip;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1) {
        std::cerr << "Error: Incorrect IP address\n";
        return 1;
    }

    // Вывод информации о порте и адресе
    // std::cout << "Port: " << ntohs(server_addr.sin_port) << std::endl
    //           << "Address: " << inet_ntoa(server_addr.sin_addr) << std::endl;


    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error: Failed to connect to server\n";
        return 1;
    }

    if (send(client_socket, name.c_str(), name.size() + 1, 0) == -1) {
        std::cerr << "Error: Failed to send message\n";
        return 1;
    }
    

    std::cout << "Connected to server\n";
    return 0;
}

int main() {
    int client_socket;
    sockaddr_in server_addr;
    
    if (Start(client_socket, server_addr) == 1) {
        return 1;
    }

    while (true) {
        std::string message;
        std::getline(std::cin, message);
        if (message == "/quit")
            break;
        if (send(client_socket, message.c_str(), message.size() + 1, 0) == -1) {
            std::cerr << "Error: Failed to send message\n";
            break;
        }
    }

    close(client_socket);

    return 0;
}