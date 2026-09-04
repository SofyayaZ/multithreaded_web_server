#include <http_server.hpp>


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

int main() {
    // Creating server socket
    int serverSocket = 0;
    if ( (serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket creating failed");
        return 1;
    };
    std::cout << "Socket has been created succesfully " << serverSocket << "\n"; 

    // Reuse address in OS
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsocket failed");
        close(serverSocket);
        return 1;
    }

    // Creating sockaddr and binding it to server socket
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return 1;
    };

    std::cout << "Server socket bound to port 8080 and all IPv4 interfaces\n";

    // Set server socket listening
    if ( listen(serverSocket, 10) < 0) {
        perror("Listening failed");
        close(serverSocket);
        return 1;
    };

    std::cout << "Socket " << serverSocket << " is listening\n";

    // Creating address for client and accepting client connection request
    struct sockaddr clientAddress = {0};
    socklen_t len = sizeof(clientAddress);
    int clientSocket = 0;
    if ( (clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &len)) < 0) {
        perror("Client connection failed");
        close(serverSocket);
        return 1;
    };

    std::cout << "Client socket has been created succesfully\n";

    // Receiving bytes from client and putting to buffer
    char buffer [4096];
    ssize_t receivedBytes = 0;
    if ( (receivedBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) < 0 ) {
        perror("Failed receiving data from client");
        close(serverSocket);
        close(clientSocket);
        return 1;
    };
    buffer[receivedBytes] = '\0';

    // Request parsing for bytes from buffer
    auto httpRequest = parseRequest(buffer);
    if (!httpRequest) {
        std::cerr << "Bad http request structure\n";
        close(serverSocket);
        close(clientSocket);
        return 1;
    }

    // Adding routes and routing request from client
    Router router = Router();
    router.addRoute("GET", "/", homeHandler);
    router.addRoute("GET", "/hello", helloHandler);
    HttpResponse response = router.route(*httpRequest);

    // Serializing response to string for sending data to client
    ResponseSerializer serializer = ResponseSerializer();
    std::string serializedResponse = serializer.serializeResponse(response);

    // Sending response to client
    if (!sendAll(clientSocket, serializedResponse)) {
        std::cerr << "Response sending failed\n";
        close(serverSocket);
        close(clientSocket);
        return 1;
    }
    std::cout << "Message succesfully sent to client " << clientSocket << "\n";

    // Closing sockets
    close(clientSocket);
    std::cout << "Client socket " << clientSocket << " has been closed\n";
    
    close(serverSocket);
    std::cout << "Server socket " << serverSocket << " has been closed\n";

    return 0;
}