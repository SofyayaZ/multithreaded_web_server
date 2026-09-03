cmake_minimum_required(VERSION 3.25)
project ("my super server" LANGUAGES CXX)
add_executable(tcp_server)

target_sources(tcp_server
               PRIVATE
               src/tcp_server.cpp
)

target_include_directories(tcp_server
                           PRIVATE
                           $(CMAKE_CURRENT_SOURCE_DIR)/include
)