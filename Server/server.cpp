#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

class Client {
public:
    std::string name = "";
    int socket = 0;
};

const int MESSAGE_SIZE = 1024;
std::vector<Client> clients;

int SendMessage(std::string& message) {
    for (Client& client : clients) {
        if (client.socket != 0) {
            if (send(client.socket, message.c_str(), message.size() + 1, 0) == -1) {
                std::cerr << "Error: Failed to send message to "
                          << client.name << "\n";
                continue;
            }
        }
    }
    return 0;
}

// Обработка клиента, запускается в отдельном потоке в функции Connect()
void HandleClient(size_t ind) {
    char buffer[MESSAGE_SIZE];
    while (true) {
        int bytes_received = recv(clients[ind].socket, buffer, MESSAGE_SIZE, 0);
        if (bytes_received == -1) {
            std::cerr << "Error: Failed to receive data\n";
            break;
        } else if (bytes_received == 0) {
            std::cout << clients[ind].name << " disconnected\n";
            break;
        }
        std::string message = clients[ind].name + ": " + std::string(buffer, bytes_received);
        SendMessage(message);
        std::cout << message << std::endl;
    }
    close(clients[ind].socket);
    clients[ind].socket = 0;
}

// Инициализация серверного сокета, связывание его с портом и прослушивание
// входящих соединений
int Start(int& server_socket, sockaddr_in& server_addr) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    uint16_t port;
    std::cout << "Port: ";
    std::cin >> port;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
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

// Принятие входящих соединений от клиентов, добавление их в вектор клиентов
void Connect(int server_socket) {
    while (true) {
        Client client;
        client.socket = accept(server_socket, nullptr, nullptr);
        if (client.socket == -1 || client.socket == 0) {
            std::cerr << "Error: Failed to accept connection\n";
            continue;
        }

        // Надо сделать ограничения для имён пользователей
        char buffer[MESSAGE_SIZE];
        int bytesReceived = recv(client.socket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            std::cerr << "Error: Failed to receive name\n";
            close(client.socket);
            continue;
        }

        client.name = buffer;
        std::cout << client.name << " connected\n";
        
        clients.push_back(client);
        std::thread client_thread(HandleClient, clients.size() - 1);
        client_thread.detach();
    }
}

int main() {
    int server_socket;
    sockaddr_in server_addr;

    if (Start(server_socket, server_addr) == 1) {
        return 1;
    }
    
    std::thread connect_thread(Connect, server_socket);
    connect_thread.join();

    for (int i = 0; i != clients.size(); ++i) {
        close(clients[i].socket);
    }
    close(server_socket);

    return 0;
}