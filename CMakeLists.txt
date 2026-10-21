cmake_minimum_required(VERSION 3.25)
project ("my super server" LANGUAGES CXX)

# Dependencies
find_package(Threads REQUIRED)

# Unit tests
add_executable(unit_tests)

# Server library
add_library(http_server_lib STATIC 
            src/adapters/adapt_handler.cpp 

            src/http/request_parser.cpp 
            src/http/response_serializer.cpp 
            src/http/send_all.cpp 
            src/http/send_response.cpp 

            src/routing/router.cpp 

            src/server/client_handler.cpp 
            src/server/http_server.cpp 
            src/server/thread_pool.cpp 
)

# Public API headers
target_include_directories(http_server_lib
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}/include 
                           PRIVATE 
                           ${PROJECT_SOURCE_DIR}/src
)

# Threads is required by implementation
target_link_libraries(http_server_lib PRIVATE Threads::Threads)

#Compile warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(http_server_lib 
                           PRIVATE 
                           -Wall 
                           -Wextra
    )
endif()

# Server executable
add_executable(http_server src/main.cpp)
target_link_libraries(http_server PRIVATE http_server_lib)

#Tests
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
include(CTest)

target_sources(unit_tests
               PRIVATE
               tests/test_client_handler.cpp
               tests/test_request_parser.cpp
               tests/test_router.cpp
)

target_include_directories(unit_tests
                           PRIVATE
                           ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(unit_tests
                      PRIVATE
                      http_server_lib
                      GTest::gtest_main
)

gtest_discover_tests(unit_tests)
