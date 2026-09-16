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
    int serverFd = 0;
    if ( (serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Socket creating failed\n");
        return 1;
    }
    std::cout << "Socket has been created succesfully " << serverFd << "\n"; 

    // Reuse address in OS
    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsocket failed\n");
        close(serverFd);
        return 1;
    }

    // Creating sockaddr and binding it to server socket
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverFd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        perror("Binding failed\n");
        close(serverFd);
        std::cout << "Server socket " << serverFd << " has been closed\n";
        return 1;
    }

    std::cout << "Server socket bound to port 8080 and all IPv4 interfaces\n";

    // Set server socket listening
    if ( listen(serverFd, 10) < 0) {
        perror("Listening failed\n");
        close(serverFd);
        std::cout << "Server socket " << serverFd << " has been closed\n";
        return 1;
    }

    std::cout << "Socket " << serverFd << " is listening\n";

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
        int clientFd = 0;

        if ( (clientFd = accept(serverFd, &clientAddress, &len)) < 0) {
            if (errno == EINTR && stop) {
                break;
            }
            std::cerr << "Failed to accept client connection\n";
            continue;
        }
        Socket clientSocket(clientFd);
        std::cout << "Client socket has been created successfully\n";
        
        // Copying of Socket is forbidden. Moving resources
        threadPool.enqueue(std::move(clientSocket));
    }

    // Closing server socket
    close(serverFd);
    std::cout << "Server socket " << serverFd << " has been closed\n";

    return 0;
}