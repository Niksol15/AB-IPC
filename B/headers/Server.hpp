//
// Created by niksol on 10.03.2021.
//

#ifndef B_SERVER_HPP
#define B_SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

#include <netdb.h>
#include <unistd.h>

class Server {
public:

    Server(std::string ip_addr, uint16_t port);

    void start();

    template<class Handler>
    void acceptConnection(Handler &&handler) {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int recv_socket = accept(m_socket, reinterpret_cast<sockaddr *>(&client), &clientSize);
        if (recv_socket == -1) {
            throw std::logic_error("Problem with connecting");
        }
        handler(recv_socket);
        close(recv_socket);
    }

    ~Server();

private:
    std::string m_ip;
    uint16_t m_port;
    int m_socket;
};


#endif //B_SERVER_HPP
