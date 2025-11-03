cmake_minimum_required(VERSION 3.16)
project(MessengerServer)

# Подпроекты
add_subdirectory(src/core)
add_subdirectory(src/network) 
add_subdirectory(src/auth)
add_subdirectory(src/protocol)

# Исполняемый файл
add_executable(messenger_server 
    src/main.cpp
    ${CORE_SOURCES}
    ${NETWORK_SOURCES}
    ${AUTH_SOURCES}
    ${PROTOCOL_SOURCES}
)
