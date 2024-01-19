cmake_minimum_required(VERSION 3.22.1)

project(Client)

set(COMMON_PATH ../Commune)

include_directories(headers)
include_directories(${COMMON_PATH}/headers)

set(SOURCES
    src/main.c
)
    
set(HEADERS
    headers/client.h
)

link_directories(${COMMON_PATH}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(Client ${SOURCES} ${HEADERS})

target_link_libraries(Client Commune)
