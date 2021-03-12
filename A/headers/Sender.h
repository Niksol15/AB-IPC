//
// Created by niksol on 11.03.2021.
//

#ifndef A_SENDER_H
#define A_SENDER_H

#include <iostream>
#include <string>
#include <cstdint>

class Sender {
public:
    Sender() noexcept;

    Sender(std::string ip, uint16_t port) noexcept;

    int run();

    ~Sender();

    static constexpr char help[] = "Input enter your message in the following format:\n"
                                   "Delay in ms(uint16_t)\n"
                                   "Message text(string)";
private:
    std::string m_ip;
    uint16_t m_port;
    int m_socket;
    std::string m_text;
    uint16_t m_delay;

    void connectToServer();

    void readMessage(std::istream & = std::cin) noexcept;

    void sendMessage(const char *buffer, size_t length) const;

    size_t createMessage(char *buffer); // return number of bytes in message;
};


#endif //A_SENDER_H
