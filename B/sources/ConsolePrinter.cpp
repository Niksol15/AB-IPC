//
// Created by niksol on 10.03.2021.
//

#include "ConsolePrinter.h"

#include <cstring>
#include <chrono>
#include <future>

#include <sys/socket.h>

namespace {
    constexpr char kDefaultMessage[] = "Hey, I am here.";
    constexpr uint16_t kDefaultDelay = 500;
    constexpr char kDefaultIP[] = "127.0.0.1";
    constexpr uint16_t kDefaultPort = 7050;
    constexpr size_t kBufferSize = 1024;
    constexpr char kClientDisconnection[] = "Unexpected client disconnection";
    constexpr char kConnectingError[] = "Problem with connecting: ";
    constexpr char kQuitMessage[] = "Quit";
    constexpr uint16_t kMinDelay = 250;
    constexpr uint16_t kMaxDelay = 2000;
    constexpr char kInvalidDelayMessage[] = "Invalid delay, nothing changes";
    constexpr char kNewMessage[] = "New message received: ";
    constexpr char kIncomingConnection[] = "Connection handled";
}

ConsolePrinter::ConsolePrinter() :
        m_text(kDefaultMessage), m_delay(kDefaultDelay),
        m_server(kDefaultIP, kDefaultPort), m_isInProcess(true) {
}

ConsolePrinter::ConsolePrinter(std::string ip, uint16_t port) :
        m_text(kDefaultMessage), m_delay(kDefaultDelay),
        m_server(move(ip), port), m_isInProcess(true) {

}

void ConsolePrinter::workFunction() {
    while (m_isInProcess) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_delay));
        if (m_isInProcess) {
            std::lock_guard<std::mutex> textLock(texMtx);
            print(m_text);
        }
    }
}

void ConsolePrinter::handleConnection(int socket) {
    print(kIncomingConnection);
    char buffer[kBufferSize];
    memset(buffer, '\0', kBufferSize);
    ssize_t bytes;
    while (true) {
        bytes = recv(socket, buffer, kBufferSize, 0);
        if (bytes > 0) {
            print(kNewMessage);
            if (!notifyAndUpdateValues(buffer)) {
                m_isInProcess = false;
                return;
            }
        } else {
            m_isInProcess = false;
            if (bytes == 0) {
                throw std::logic_error(kClientDisconnection);
            } else if (bytes == -1) {
                throw std::logic_error(std::string(kConnectingError) + strerror(errno));
            }
            return;
        }
        memset(buffer, '\0', bytes);
    }
}

int ConsolePrinter::run() noexcept {
    try {
        m_server.start();
        std::future<void> asyncHandler = async(std::launch::async, [this]() {
            m_server.acceptConnection([this](int socket) {
                handleConnection(socket);
            });
        });
        std::future<void> asyncPrinter = async(std::launch::async, [this]() {
            workFunction();
        });
        asyncPrinter.get();
        asyncHandler.get();
    }
    catch (std::exception &e) {
        std::cout << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

bool ConsolePrinter::notifyAndUpdateValues(char *buffer) {
    uint16_t recvDelay = 0;
    memcpy(&recvDelay, buffer, sizeof(recvDelay));
    recvDelay = ntohs(recvDelay);
    std::string notifyMessage;
    notifyMessage.reserve(kBufferSize);
    notifyMessage += "Delay: ";
    notifyMessage += std::to_string(recvDelay);
    notifyMessage += "\nText: ";
    std::string recvText = buffer + sizeof(recvDelay);
    notifyMessage += recvText;
    print(notifyMessage);
    if (recvText == kQuitMessage) {
        return false;
    } else if (setDelay(recvDelay)) {
        setText(std::move(recvText));
    } else {
        print(kInvalidDelayMessage);
    }
    return true;
}

bool ConsolePrinter::setDelay(uint16_t recvDelay) noexcept {
    if (recvDelay >= kMinDelay && recvDelay <= kMaxDelay) {
        m_delay = recvDelay;
        return true;
    }
    return false;
}

void ConsolePrinter::setText(char *msg) {
    std::lock_guard<std::mutex> textLock(texMtx);
    m_text = msg;
}

void ConsolePrinter::setText(std::string msg) noexcept {
    std::lock_guard<std::mutex> textLock(texMtx);
    m_text = std::move(msg);
}

void ConsolePrinter::print(const std::string &text, std::ostream &os) {
    std::lock_guard<std::mutex> osLock(osMtx);
    os << text << std::endl;
}