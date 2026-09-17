#include <send_all.hpp>

#include <iostream>
#include <sys/socket.h>


bool sendAll(int clientSocket, const std::string& response) {
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Socket option setting failed\n";
        return false;
    }

    ssize_t sentData = 0;
    size_t totalSent = 0;
    while (totalSent < response.size()) {
        if ( (sentData = send(clientSocket,
                         response.data() + totalSent,
                         response.size() - totalSent,
                         0)) <= 0) {
    
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "The time for sending the response to client ";
                std::cout << clientSocket << " has expired\n";
            } else {
                std::cerr << "The sending to client " << clientSocket << " failed\n";
            }
            return false;
        }
        totalSent += sentData; 
    }

    return true;
};
