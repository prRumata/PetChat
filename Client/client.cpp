#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>


const int MESSAGE_SIZE = 1024;

// Инициализация клиентского сокета и подключение его к серверу
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
    std::cin >> name;
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
        close(client_socket);
        return 1;
    }

    if (send(client_socket, name.c_str(), name.size() + 1, 0) == -1) {
        std::cerr << "Error: Failed to send message\n";
        return 1;
    }
    

    std::cout << "Connected to server\n";
    return 0;
}

void SendMessage(int client_socket) {
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
    std::exit(0);
}

void GetMessage(int client_socket) {
    char buffer[MESSAGE_SIZE];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Error: Failed to receive data\n";
            break;
        } else if (bytes_received == 0) {
            std::cout << "Server disconnected\n";
            break;
        }
        std::cout << std::string(buffer, bytes_received) << std::endl;
    }
    close(client_socket);
    exit(0);
}

int main() {
    int client_socket;
    sockaddr_in server_addr;
    std::string name;
    
    if (Start(client_socket, server_addr) == 1) {
        return 1;
    }

    std::thread send_thread(SendMessage, client_socket);
    std::thread recv_thread(GetMessage, client_socket);
    
    send_thread.join();
    recv_thread.join();

    return 0;
}