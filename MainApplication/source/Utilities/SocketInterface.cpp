#include "SocketInterface.h"

SocketInterface::SocketInterface(const std::string& ipAddress, uint16_t port)
{
    m_ipAddress = ipAddress;
    m_port = port;

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
}

bool SocketInterface::SendCommand(const std::string& command, std::string& outResponse, size_t responseMaxSize)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(m_port);
    inet_pton(AF_INET, m_ipAddress.c_str(), &addr.sin_addr);

    ssize_t socketResult = sendto(m_socket, command.c_str(), command.size(), 0, (sockaddr*)&addr, sizeof(addr));

    if(socketResult <= 0)
    {
        return false;
    }

    std::vector<char> responseBuffer(responseMaxSize);
    timeval tv{kTimeoutLength, 0};
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    socketResult = recv(m_socket, responseBuffer.data(), responseBuffer.size() - 1, 0);

    if (socketResult <= 0) 
    { 
        return false;
    }

    outResponse = std::string(responseBuffer.begin(), responseBuffer.end());
    return true;
}