#include <send_all.hpp>


bool sendAll(int clientSocket, const std::string& response) {
    ssize_t sentData = 0;
    size_t totalSent = 0;
    while (totalSent < response.size()) {
        if ( (sentData = send(clientSocket,
                         response.data() + totalSent,
                         response.size() - totalSent,
                         0)) <= 0) {
            return false;
        }
        totalSent += sentData; 
    }

    return true;
};
