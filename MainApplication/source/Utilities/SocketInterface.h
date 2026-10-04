#ifndef POINTCLOUDAPP_SOCKETINTERFACE_H
#define POINTCLOUDAPP_SOCKETINTERFACE_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <vector>

class SocketInterface
{
public:
    SocketInterface(const std::string& ipAddress, uint16_t port);

    bool SendCommand(const std::string& command, std::string& outResponse, size_t responseMaxSize=6);

private:
    static constexpr size_t kTimeoutLength = 10;

    std::string m_ipAddress = "";
    uint16_t m_port = 0;

    int m_socket = 0;
};

#endif