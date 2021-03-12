//
// Created by niksol on 10.03.2021.
//

#include "Server.hpp"

#include <cstring>
#include <stdexcept>


#include <sys/socket.h>
#include <arpa/inet.h>


namespace {
    constexpr char kCantCreateASocketError[] = "Can`t create a socket!";
    constexpr char kCantBindError[] = "Can`t bind to IP port!";
    constexpr char kCantListenError[] = "Can`t listen";
    constexpr char kConnectingError[] = "Problem with connecting: ";
    constexpr std::size_t kMaxNumOfConnection = SOMAXCONN;
}

Server::Server(std::string ip_addr, uint16_t port) : m_ip(std::move(ip_addr)), m_port(port),
                                                     m_socket(-1) {
}

void Server::start() {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) {
        throw std::logic_error(kCantCreateASocketError);
    }
    sockaddr_in addr{.sin_family = AF_INET, .sin_port = htons(m_port)};
    inet_pton(AF_INET, m_ip.c_str(), &addr.sin_addr);
    if (bind(m_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::logic_error(kCantBindError);
    }
    if (listen(m_socket, kMaxNumOfConnection) == -1) {
        throw std::logic_error(kCantListenError);
    }
}

Server::~Server() {
    shutdown(m_socket, SHUT_RDWR);
    close(m_socket);
}