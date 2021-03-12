//
// Created by niksol on 11.03.2021.
//

#include "Sender.h"

#include <cstring>
#include <utility>
#include <stdexcept>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>


namespace {
    constexpr char kDefaultIP[] = "127.0.0.1";
    constexpr uint16_t kDefaultPort = 7050;
    constexpr char kCantCreateASocketError[] = "Can`t create a socket!";
    constexpr char kCantConnectError[] = "Can`t connect to server port!";
    constexpr char kCantSendError[] = "Can`t send a message to server";
    constexpr char kTooLongMessageError[] = "Message is too long";
    constexpr char kQuitMessage[] = "Quit";
    constexpr size_t kBufferSize = 1024;
    constexpr char kCinProblem[] = "Problem with cin";
}

Sender::Sender() noexcept: m_ip(kDefaultIP), m_port(kDefaultPort), m_socket(-1), m_delay(0) {
}

Sender::Sender(std::string ip, uint16_t port) noexcept:
        m_ip(std::move(ip)), m_port(port), m_socket(-1), m_delay(0) {

}

constexpr char Sender::help[];

int Sender::run() {
    try {
        connectToServer();
        char buffer[kBufferSize];
        memset(buffer, '\0', kBufferSize);
        size_t messageLength;
        while (m_text != kQuitMessage && std::cin) {
            readMessage();
            try {
                messageLength = createMessage(buffer);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                continue;
            }
            sendMessage(buffer, messageLength);
            std::cout << "Send Message:\nText: " << m_text
                      << "\nDelay: " << m_delay << std::endl;
            memset(buffer, '\0', messageLength);
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    if (!std::cin) {
        std::cout << kCinProblem << std::endl;
        return 1;
    }
    return 0;
}

void Sender::connectToServer() {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) {
        throw std::logic_error(kCantCreateASocketError);
    }
    sockaddr_in addr{.sin_family = AF_INET, .sin_port = htons(m_port)};
    inet_pton(AF_INET, m_ip.c_str(), &addr.sin_addr);
    if (connect(m_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
        throw std::logic_error(kCantConnectError);
    }
}

void Sender::readMessage(std::istream &is) noexcept {
    is >> m_delay;
    is.get();
    std::getline(is, m_text);
}

void Sender::sendMessage(const char *buffer, size_t length) const {
    ssize_t bytes = send(m_socket, buffer, length, 0);
    if (bytes == -1) {
        throw std::logic_error(kCantSendError);
    }
}

size_t Sender::createMessage(char *buffer) {
    size_t length = m_text.size() + sizeof(uint16_t);
    if (length > kBufferSize) {
        throw std::logic_error(kTooLongMessageError);
    }
    uint16_t delayToSend = htons(m_delay);
    memcpy(buffer, &delayToSend, sizeof(delayToSend));
    memcpy(buffer + sizeof(delayToSend), m_text.c_str(), m_text.size());
    return length;
}

Sender::~Sender() {
    shutdown(m_socket, SHUT_RD);
    close(m_socket);
}