#include <http_server.hpp>


void handleSignal(int signal) {
    if (signal == SIGINT) {
        stop = 1;
    }
}

int main() {
    // For handling signals
    struct sigaction sa{};
    sa.sa_handler = &handleSignal;
    sigemptyset(&sa.sa_mask);     // do not block extra signals
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Failed to call sigaction");
        return 1;
    }

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
    router.addRoute("POST", "/hello", postHelloHandler);
    //Creating serializer (HttpResponse -> std::string)
    ResponseSerializer serializer = ResponseSerializer();

    // ThreadPool for accepting clients' connections
    ThreadPool threadPool = ThreadPool(4, router, serializer);

    // Handling client's requests
    while(!stop) {
        // Creating address for client and accepting client connection request
        sockaddr clientAddress{};
        socklen_t len = sizeof(clientAddress);
        int clientSocket = 0;

        if ( (clientSocket = accept(serverSocket, &clientAddress, &len)) < 0) {
            if (errno == EINTR && stop) {
                break;
            }
            std::cerr << "Failed to accept client connection\n";
            continue;
        }
        std::cout << "Client socket has been created successfully\n";
        
        threadPool.enqueue(clientSocket);
    }

    // Closing server socket
    close(serverSocket);
    std::cout << "Server socket " << serverSocket << " has been closed\n";

    return 0;
}