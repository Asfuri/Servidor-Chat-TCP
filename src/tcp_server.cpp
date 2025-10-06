#include "../lib/libtslog.h"
#include "../lib/message_history.h"
#include "../lib/socket_guard.h"
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <string>
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

        std::atomic<bool> running{true};

public:
        TCPChatServer(int p = 8080) : port(p), messageHistory(100) {
        }

        ~TCPChatServer() {
                shutdown();
        }

        void shutdown() {
                if (!running.exchange(false)) {
                        return; // Já foi encerrado
                }

                logger.log("Servidor encerrando...");

                // Desconectar todos os clientes
                {
                        std::lock_guard<std::mutex> lock(clientsMutex);
                        for (auto& client : clients) {
                                if (client->socket >= 0) {
                                        ::shutdown(client->socket, SHUT_RDWR);
                                        close(client->socket);
                                }
                        }
                        clients.clear();
                }

                // Fechar socket do servidor
                if (serverSocket >= 0) {
                        ::shutdown(serverSocket, SHUT_RDWR);
                        close(serverSocket);
                        serverSocket = -1;
                }

                logger.log("Servidor encerrado");
                        std::cout << "\n✅ Servidor encerrado" << std::endl;
        }

        void commandLoop() {
                std::cout << "\n=== Servidor TCP Iniciado ===" << std::endl;
                std::cout << "Digite 'sair' para encerrar o servidor" << std::endl;
                std::cout << "Digite 'help' para comandos" << std::endl;
                std::cout << "É interessante o uso de 'make logs-tail' para ver os logs do servidor" << std::endl;
                std::cout << "'make help' para ver todos os comandos de make" << std::endl;
                std::cout << "============================\n"
                          << std::endl;

                        std::string command;

                while (running) {
                        std::cout << "[servidor] > " << std::flush;


                        if (!std::getline(std::cin, command) || std::cin.eof()) {
                                // Sem stdin (modo background): sai só da thread de comandos
                                logger.log("Entrada padrão indisponível - executando sem console");
                                return; // não altera 'running'
                            }

                        if (!running)
                                break;

                        if (command.empty())
                                continue;
                        
                        if (command == "sair") {
                                std::cout << "Encerrando servidor..." << std::endl;
                                shutdown();
                                break;
                        } else if (command == "status") {
                                std::lock_guard<std::mutex> lock(clientsMutex);
                                std::cout << "Clientes conectados: " << clients.size() << std::endl;
                                std::cout << "Mensagens no histórico: " << messageHistory.size() << std::endl;
                        } else if (command == "help") {
                                std::cout << "Comandos disponíveis:" << std::endl;
                                std::cout << "  status   - Mostra número de clientes conectados" << std::endl;
                                std::cout << "  sair - Encerra o servidor" << std::endl;
                                std::cout << "  help     - Mostra esta mensagem" << std::endl;
                        } else if (!command.empty()) {
                                std::cout << "Comando desconhecido. Digite 'help' para ver comandos." << std::endl;
                        }
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
                logger.log("Servidor ouvindo conexões na porta " + std::to_string(port));

                // Liberar o socket para uso contínuo
                serverSocket = serverSock.release();

                std::thread commandThread(&TCPChatServer::commandLoop, this);
                commandThread.detach();

                // Loop principal de accept
                while (running) {
                        sockaddr_in clientAddr{};
                        socklen_t clientLen = sizeof(clientAddr);

                        fd_set readfds;
                        FD_ZERO(&readfds);
                        FD_SET(serverSocket, &readfds);

                        struct timeval tv;
                        tv.tv_sec = 1; // Timeout de 1 segundo
                        tv.tv_usec = 0;

                        int activity = select(serverSocket + 1, &readfds, NULL, NULL, &tv);

                        if (activity < 0) {
                                if (running) {
                                        logger.log("ERRO: Select falhou");
                                }
                                break;
                        }

                        if (activity == 0) {
                                // Timeout - continua loop para verificar running
                                continue;
                        }

                        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);

                        if (clientSocket < 0) {
                                if (running) {
                                        logger.log("ERRO: Accept falhou");
                                }
                                continue;
                        }

                        if (!running) {
                                close(clientSocket);
                                break;
                        }

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

                        // Criar thread com lambda
                        client->thread = std::make_unique<std::thread>(
                            [this, client]() { handleClient(client); });
                        client->thread->detach();
                }

                logger.log("Loop principal do servidor encerrado");

                if (commandThread.joinable()) {
                        commandThread.join();
                }
        }

private:
        void handleClient(std::shared_ptr<ClientInfo> client) {
                // RAII: Socket fechado automaticamente ao sair do escopo
                SocketGuard sockGuard(client->socket);

                logger.log("Thread iniciada para Cliente " + std::to_string(client->clientId));

                char buffer[1024];

                while (running) {
                        int bytesRead = recv(sockGuard.get(), buffer, sizeof(buffer) - 1, 0);

                        if (bytesRead <= 0) {
                                logger.log("Cliente " + std::to_string(client->clientId) + " desconectado");
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

                        logger.log("Mensagem recebida do Cliente " + std::to_string(client->clientId) + ": " + message);

                        // Retransmitir
                        broadcastMessage(message, sockGuard.get());
                }
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
