cmake_minimum_required(VERSION 3.16)
project(B LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_CXX_FLAGS "-pthread -Wall")

set(HEADERS_DIR headers)

set(SOURCES_DIR sources)

list(APPEND HEADERS  ${HEADERS_DIR}/ConsolePrinter.h
        ${HEADERS_DIR}/Server.hpp)

list(APPEND SOURCES ${SOURCES_DIR}/main.cpp
        ${SOURCES_DIR}/ConsolePrinter.cpp
        ${SOURCES_DIR}/Server.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_DIR})