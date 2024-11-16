#include <iostream>
#include <string>
#include <winsock2.h>
#include "communicate.h"
#pragma comment(lib, "ws2_32.lib")

ESP32Communicator::ESP32Communicator() : sock(INVALID_SOCKET), serv_addr{} {}

ESP32Communicator::~ESP32Communicator() {
    cleanup();
}

bool ESP32Communicator::initialize(const std::string& ip, int port) 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    return true;
}

bool ESP32Communicator::connectToServer() 
{
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }
    return true;
}

bool ESP32Communicator::sendCommand(const std::string& command) 
{
    if (send(sock, command.c_str(), command.length(), 0) < 0) 
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Sent command: " << command << std::endl;
    return true;
}

void ESP32Communicator::cleanup() 
{
    if (sock != INVALID_SOCKET) 
    {
        closesocket(sock);
        WSACleanup();
    }
}
