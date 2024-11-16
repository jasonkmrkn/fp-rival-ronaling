#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <WinSock2.h>
#include <string>

class ESP32Communicator {
private:
    SOCKET sock;
    sockaddr_in serv_addr;

public:
    ESP32Communicator(); 
    ~ESP32Communicator(); 

    // Initialization method
    bool initialize(const std::string& ip, int port);

    // Method to connect to the ESP32
    bool connectToServer();

    // Method to send a command
    bool sendCommand(const std::string& command);

    // Cleanup resources
    void cleanup();
};

#endif