#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
public:
    std::string name;
    int socket;
};

int Start(int& server_socket, sockaddr_in& server_addr) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    uint16_t port;
    std::cout << "Port: ";
    std::cin >> port;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Вывод информации о порте и адресе
    // std::cout << "Port: " << ntohs(server_addr.sin_port) << std::endl
    //           << "Address: " << inet_ntoa(server_addr.sin_addr) << std::endl;


    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error: Failed to bind socket\n";
        return 1;
    }

    // 5 - backlog, определяет максимальное количество
    // ожидающих соединений в очереди
    if (listen(server_socket, 5) == -1) {
        std::cerr << "Error: Failed to listen on socket\n";
        return 1;
    }



    std::cout << "Server started. Waiting for connections...\n";
    return 0;
}

int Connect(std::vector<Client>& clients, int& server_socket) {
    clients.push_back({"", 0});
    int i = clients.size() - 1;
    clients[i].socket = accept(server_socket, nullptr, nullptr);
    if (clients[i].socket == -1) {
        std::cerr << "Error: Failed to accept connection\n";
        return 1;
    }

    char buffer[1024];
    int bytesReceived = recv(clients[i].socket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        std::cerr << "Error: Failed to receive name\n";
        close(clients[i].socket);
        return 1;
    }

    clients[i].name = buffer;

    std::cout << clients[i].name << " connected!\n";
    return 0;
}

int main() {
    std::vector<Client> clients;
    int server_socket;
    sockaddr_in server_addr;

    if (Start(server_socket, server_addr) == 1) {
        return 1;
    }

    if (Connect(clients, server_socket) == 1) {
        return 1;
    }
    
    while (true) {
        char buffer[1024];
        for (int i = 0; i != clients.size(); ++i) {
            int bytes_received = recv(clients[i].socket, buffer, sizeof(buffer), 0);
            if (bytes_received == -1) {
                std::cerr << "Error: Failed to receive data\n";
                break;
            } else if (bytes_received == 0) {
                std::cout << clients[i].name << " disconnected\n";
                break;
            }
            std::cout << clients[i].name << ": " 
                    << std::string(buffer, bytes_received) << std::endl;
        }
        std::cout << "\n";
    }

    for (int i = 0; i != clients.size(); ++i) {
        close(clients[i].socket);
    }
    close(server_socket);

    return 0;
}