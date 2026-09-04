cmake_minimum_required(VERSION 3.25)
project ("my super server" LANGUAGES CXX)
add_executable(http_server)

add_library(router)
add_library(request_parser)
add_library(response_serializer)
add_library(project_headers INTERFACE)

target_sources(http_server
               PRIVATE
               src/http_server.cpp
)

target_sources(router
               PRIVATE
               src/router.cpp
)

target_sources(request_parser
               PRIVATE
               src/request_parser.cpp
)

target_sources(response_serializer
               PRIVATE
               src/response_serializer.cpp
)

target_include_directories(project_headers
                           INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(router
                      PUBLIC
                      project_headers
)

target_link_libraries(request_parser
                      PUBLIC
                      project_headers
)

target_link_libraries(response_serializer
                      PUBLIC
                      project_headers
)

target_link_libraries(http_server
                      PRIVATE
                      router
                      request_parser
                      response_serializer
)