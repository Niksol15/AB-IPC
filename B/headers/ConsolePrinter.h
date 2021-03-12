//
// Created by niksol on 10.03.2021.
//

#ifndef B_CONSOLEPRINTER_H
#define B_CONSOLEPRINTER_H

#include "Server.hpp"

#include <atomic>
#include <mutex>
#include <string>
#include <iostream>

class ConsolePrinter {
public:
    ConsolePrinter();

    ConsolePrinter(std::string ip, uint16_t port);

    int run() noexcept;

private:
    std::mutex osMtx;
    std::mutex texMtx;
    std::string m_text;
    std::atomic<uint16_t> m_delay;
    Server m_server;
    std::atomic<bool> m_isInProcess;

    void workFunction();

    void print(const std::string &text, std::ostream &os = std::cout);

    void handleConnection(int socket);

    void setText(char *text);

    void setText(std::string text) noexcept;

    bool setDelay(uint16_t recvValue) noexcept; // return true if delay valid;

    bool notifyAndUpdateValues(char *buffer); // return msg != Quit message
};


#endif //B_CONSOLEPRINTER_H
