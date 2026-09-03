cmake_minimum_required(VERSION 3.25)
project ("my super server" LANGUAGES CXX)
add_executable(tcp_server)
add_library(router)

target_sources(tcp_server
               PRIVATE
               src/tcp_server.cpp
)

target_sources(router
               PRIVATE
               src/router.cpp
)

target_link_libraries(tcp_server
                      PRIVATE
                      router
)

target_include_directories(router
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
)