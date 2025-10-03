#include "../lib/libtslog.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <algorithm>

class TCPChatServer {
private:
        int serverSocket;
        int port;
        ThreadSafeLogger logger;
        std::vector<int> clientSockets;
        std::mutex clientsMutex;

public:
        TCPChatServer(int p = 8080) : port(p) {
        }

        void start() {
                logger.initialize("logs/server.log");
                logger.log("Servidor iniciando na porta " + std::to_string(port));

                // Criar socket
                serverSocket = socket(AF_INET, SOCK_STREAM, 0);

                // Configurar endereço
                sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_addr.s_addr = INADDR_ANY;
                serverAddr.sin_port = htons(port);

                // Bind e Listen
                bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
                listen(serverSocket, 5);

                logger.log("Servidor ouvindo conexões...");

                // Loop principal
                while (true) {
                        sockaddr_in clientAddr;
                        socklen_t clientLen = sizeof(clientAddr);
                        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);

                        if (clientSocket > 0) {
                                logger.log("Cliente conectado: " + std::to_string(clientSocket));

                                // Adicionar à lista de clientes
                                {
                                        std::lock_guard<std::mutex> lock(clientsMutex);
                                        clientSockets.push_back(clientSocket);
                                }

                                // Criar thread para cliente
                                std::thread clientThread(&TCPChatServer::handleClient, this, clientSocket);
                                clientThread.detach();
                        }
                }
        }

        void handleClient(int clientSocket) {
                char buffer[1024];

                while (true) {
                        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

                        if (bytesRead <= 0) {
                                logger.log("Cliente desconectado: " + std::to_string(clientSocket));
                                removeClient(clientSocket);
                                break;
                        }

                        buffer[bytesRead] = '\0';
                        std::string message(buffer);

                        logger.log("Mensagem recebida de " + std::to_string(clientSocket) + ": " + message);

                        // Retransmitir para todos os outros clientes
                        broadcastMessage(message, clientSocket);
                }

                close(clientSocket);
        }

        void broadcastMessage(const std::string& message, int senderSocket) {
                std::lock_guard<std::mutex> lock(clientsMutex);

                std::string fullMessage = "Cliente " + std::to_string(senderSocket) + ": " + message;

                for (int socket : clientSockets) {
                        if (socket != senderSocket) {
                                send(socket, fullMessage.c_str(), fullMessage.length(), 0);
                        }
                }

                logger.log("Mensagem retransmitida: " + fullMessage);
        }

        void removeClient(int clientSocket) {
                std::lock_guard<std::mutex> lock(clientsMutex);
                clientSockets.erase(
                    std::remove(clientSockets.begin(), clientSockets.end(), clientSocket),
                    clientSockets.end());
        }
};

int main() {
        TCPChatServer server(8080);
        server.start();
}
