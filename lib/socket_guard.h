#ifndef SOCKET_GUARD_H
#define SOCKET_GUARD_H

#include <unistd.h>

// RAII wrapper para sockets
class SocketGuard {
private:
        int socket_fd;
        bool released;

public:
        explicit SocketGuard(int fd) : socket_fd(fd), released(false) {
        }

        // Construtor de movimento
        SocketGuard(SocketGuard&& other) noexcept
            : socket_fd(other.socket_fd), released(other.released) {
                other.released = true;
        }

        // Delete copy
        SocketGuard(const SocketGuard&) = delete;
        SocketGuard& operator=(const SocketGuard&) = delete;

        // Destrutor fecha socket automaticamente
        ~SocketGuard() {
                if (!released && socket_fd >= 0) {
                        close(socket_fd);
                }
        }

        int get() const {
                return socket_fd;
        }

        // Libera controle do socket (para não fechar)
        int release() {
                released = true;
                return socket_fd;
        }

        bool is_valid() const {
                return socket_fd >= 0 && !released;
        }
};

#endif // SOCKET_GUARD_H
