cmake_minimum_required(VERSION 3.25)
project ("my super server" LANGUAGES CXX)
enable_testing()

# GoogleTest external library
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Executable + test
add_executable(http_server)
add_executable(unit_tests)

# Internal libraries
add_library(router)
add_library(request_parser)
add_library(response_serializer)
add_library(send_all)
add_library(client_handler)
add_library(thread_pool)
add_library(project_headers INTERFACE)

# Target executable
target_sources(http_server
               PRIVATE
               src/http_server.cpp
)

target_sources(unit_tests
               PRIVATE
               tests/test_client_handler.cpp
               tests/test_request_parser.cpp
               tests/test_router.cpp
)

# Target libraries
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

target_sources(send_all
               PRIVATE
               src/send_all.cpp
)

target_sources(client_handler
               PRIVATE
               src/client_handler.cpp
)

target_sources(thread_pool
               PRIVATE
               src/thread_pool.cpp
)

# Include directory
target_include_directories(project_headers
                           INTERFACE
                           ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(unit_tests
                      PRIVATE
                      client_handler
                      GTest::gtest_main
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

target_link_libraries(send_all
                      PUBLIC
                      project_headers
)

target_link_libraries(thread_pool
                      PUBLIC
                      project_headers
                      client_handler
)

target_link_libraries(client_handler
                      PUBLIC
                      router
                      request_parser
                      response_serializer
                      send_all
)

target_link_libraries(http_server
                      PRIVATE
                      thread_pool
)

gtest_discover_tests(unit_tests)
