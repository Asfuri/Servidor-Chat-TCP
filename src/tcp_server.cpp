#include "../lib/libtslog.h"
#include "../lib/message_history.h"
#include "../lib/socket_guard.h"
#include <algorithm>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

// Estrutura para gerenciar informações do cliente
struct ClientInfo {
        int socket;
        int clientId;
        std::unique_ptr<std::thread> thread;

        ClientInfo(int sock, int id) : socket(sock), clientId(id) {
        }
};

class TCPChatServer {
private:
        int serverSocket;
        int port;
        ThreadSafeLogger logger;
        MessageHistory messageHistory;

        // Usando shared_ptr para gerenciar clientes
        std::vector<std::shared_ptr<ClientInfo>> clients;
        std::mutex clientsMutex;
        int nextClientId = 1;

public:
        TCPChatServer(int p = 8080) : port(p), messageHistory(100) {
        }

        ~TCPChatServer() {
                // RAII: Cleanup automático
                if (serverSocket >= 0) {
                        close(serverSocket);
                }
        }

        void start() {
                logger.initialize("logs/server.log");
                logger.log("Servidor iniciando na porta " + std::to_string(port));

                // RAII: Socket será fechado automaticamente em caso de exceção
                SocketGuard serverSock(socket(AF_INET, SOCK_STREAM, 0));

                if (!serverSock.is_valid()) {
                        logger.log("ERRO: Falha ao criar socket");
                        return;
                }

                // Permitir reutilização rápida da porta
                int opt = 1;
                setsockopt(serverSock.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

                // Configurar endereço
                sockaddr_in serverAddr{};
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_addr.s_addr = INADDR_ANY;
                serverAddr.sin_port = htons(port);

                // Bind e Listen
                if (bind(serverSock.get(), (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                        logger.log("ERRO: Falha no bind");
                        return;
                }

                listen(serverSock.get(), 10);
                logger.log("Servidor ouvindo conexões...");

                // Liberar o socket para uso contínuo
                serverSocket = serverSock.release();

                // Loop principal
                while (true) {
                        sockaddr_in clientAddr{};
                        socklen_t clientLen = sizeof(clientAddr);
                        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);

                        if (clientSocket > 0) {
                                int clientId = nextClientId++;
                                logger.log("Cliente " + std::to_string(clientId) + " conectado (socket: " + std::to_string(clientSocket) + ")");

                                // Criar ClientInfo com smart pointer
                                auto client = std::make_shared<ClientInfo>(clientSocket, clientId);

                                {
                                        std::lock_guard<std::mutex> lock(clientsMutex);
                                        clients.push_back(client);
                                }

                                // Enviar histórico
                                sendHistoryToClient(clientSocket);

                                // Criar thread com lambda capturando shared_ptr por valor
                                client->thread = std::make_unique<std::thread>(
                                    [this, client]() { handleClient(client); });
                                client->thread->detach();
                        }
                }
        }

private:
        void handleClient(std::shared_ptr<ClientInfo> client) {
                // RAII: Socket fechado automaticamente ao sair do escopo
                SocketGuard sockGuard(client->socket);

                logger.log("Thread iniciada para Cliente " + std::to_string(client->clientId) + " (socket: " + std::to_string(sockGuard.get()) + ")");

                char buffer[1024];

                while (true) {
                        int bytesRead = recv(sockGuard.get(), buffer, sizeof(buffer) - 1, 0);

                        if (bytesRead <= 0) {
                                logger.log("Cliente " + std::to_string(client->clientId) + " desconectado (socket: " + std::to_string(sockGuard.get()) + ")");
                                removeClient(sockGuard.get());
                                break;
                        }

                        buffer[bytesRead] = '\0';
                        std::string message(buffer);

                        // Remover \r e \n do final
                        while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
                                message.pop_back();
                        }

                        if (message.empty())
                                continue;

                        logger.log("Mensagem recebida do Cliente " + std::to_string(client->clientId) + " (socket: " + std::to_string(sockGuard.get()) + "): " + message);

                        // Retransmitir
                        broadcastMessage(message, sockGuard.get());
                }

                // Socket fecha automaticamente aqui (RAII)
        }

        void broadcastMessage(const std::string& message, int senderSocket) {
                std::lock_guard<std::mutex> lock(clientsMutex);

                // Encontrar o clientId do socket
                int senderClientId = 0;
                for (const auto& client : clients) {
                        if (client->socket == senderSocket) {
                                senderClientId = client->clientId;
                                break;
                        }
                }

                std::string fullMessage = "Cliente " + std::to_string(senderClientId) + ": " + message;

                // Adicionar ao histórico
                messageHistory.addMessage(fullMessage, senderSocket);

                // Adicionar \n para framing
                fullMessage += "\n";

                // Usar range-based for com smart pointers
                for (const auto& client : clients) {
                        if (client->socket != senderSocket) {
                                send(client->socket, fullMessage.c_str(), fullMessage.length(), 0);
                        }
                }

                logger.log("Mensagem retransmitida: " + fullMessage);
        }

        void sendHistoryToClient(int clientSocket) {
                auto history = messageHistory.getRecentMessages(10);

                std::string historyMsg;

                if (history.empty()) {
                        historyMsg = "=== Bem-vindo ao chat! Seja o primeiro a enviar uma mensagem. ===\n";
                } else {
                        historyMsg = "=== Últimas " + std::to_string(history.size()) + " mensagens ===\n";
                        for (const auto& msg : history) {
                                historyMsg += msg + "\n";
                        }
                        historyMsg += "===========================\n";
                }

                send(clientSocket, historyMsg.c_str(), historyMsg.length(), 0);
                logger.log("Histórico enviado ao cliente " + std::to_string(clientSocket));
        }

        void removeClient(int clientSocket) {
                std::lock_guard<std::mutex> lock(clientsMutex);

                // Usar remove_if com lambda
                clients.erase(
                    std::remove_if(clients.begin(), clients.end(),
                                   [clientSocket](const auto& client) {
                                           return client->socket == clientSocket;
                                   }),
                    clients.end());
        }
};

int main() {
        try {
                TCPChatServer server(8080);
                server.start();
        } catch (const std::exception& e) {
                std::cerr << "Erro fatal: " << e.what() << std::endl;
                return 1;
        }

        return 0;
}
