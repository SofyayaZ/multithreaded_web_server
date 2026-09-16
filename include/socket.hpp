#pragma once
#include <unistd.h>


class Socket {
    int fd;
public:
    explicit Socket(int fd):
        fd(fd) {}

    ~Socket() {
        if (fd != -1) {
            close(fd);
        }
    }

    // Socket can have just one owner =>
    // need to forbidden copying => = delete
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept:
        fd(other.fd) {
        // -1 is invalid fd
        other.fd = -1;
    }

    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (this->fd != -1) {
                close(this->fd);
            }
            this->fd = other.fd;
            other.fd = -1;
        }
        return *this;
    }

    int get() const noexcept {
        return fd;
    }
};