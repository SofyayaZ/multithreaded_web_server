#include <http_server.hpp>


int main() {
    // Creating server socket
    int serverSocket = 0;
    if ( (serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket creating failed\n");
        return 1;
    }
    std::cout << "Socket has been created succesfully " << serverSocket << "\n"; 

    // Reuse address in OS
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsocket failed\n");
        close(serverSocket);
        return 1;
    }

    // Creating sockaddr and binding it to server socket
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("Binding failed\n");
        close(serverSocket);
        std::cout << "Server socket " << serverSocket << " has been closed\n";
        return 1;
    }

    std::cout << "Server socket bound to port 8080 and all IPv4 interfaces\n";

    // Set server socket listening
    if ( listen(serverSocket, 10) < 0) {
        perror("Listening failed\n");
        close(serverSocket);
        std::cout << "Server socket " << serverSocket << " has been closed\n";
        return 1;
    }

    std::cout << "Socket " << serverSocket << " is listening\n";

    // Creating router and adding routes
    Router router = Router();
    router.addRoute("GET", "/", homeHandler);
    router.addRoute("GET", "/hello", helloHandler);
    //Creating serializer (HttpResponse -> std::string)
    ResponseSerializer serializer = ResponseSerializer();

    // Handling client's requests
    while(true) {
        // Creating address for client and accepting client connection request
        sockaddr clientAddress{};
        socklen_t len = sizeof(clientAddress);
        int clientSocket = 0;

        //////////////////////////////////////
        //Здесь в дальнейшем будут потоки...//
        //////////////////////////////////////
        if ( (clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &len)) < 0) {
            std::cerr << "Client connection failed\n";
            close(clientSocket);
            continue;
        }
        std::cout << "Client socket " << clientSocket << " has been created succesfully\n";
        
        // Handling client (recv, send bytes)
        if (!handleClient(clientSocket, router, serializer)) {
            std::cerr << "Client handling failed\n";
            close(clientSocket);
            continue;
        }

        // Closing client socket
        close(clientSocket);
        std::cout << "Client socket " << clientSocket << " has been closed\n";
    }

    // Closing server socket
    close(serverSocket);
    std::cout << "Server socket " << serverSocket << " has been closed\n";

    return 0;
}