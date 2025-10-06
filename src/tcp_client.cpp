#include "../lib/socket_guard.h"
#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <csignal>

// Mutex global para proteger std::cout
static std::mutex coutMutex;

class TCPChatClient {
private:
        std::unique_ptr<SocketGuard> clientSocket;
        std::string serverIP;
        int serverPort;
        bool running;
        std::unique_ptr<std::thread> receiveThread;

public:
        TCPChatClient(const std::string& ip = "127.0.0.1", int port = 8080)
            : serverIP(ip), serverPort(port), running(false) {
        }

        ~TCPChatClient() {
                running = false;

                if (clientSocket && clientSocket->is_valid()) {
                        shutdown(clientSocket->get(), SHUT_RDWR);
                }

                // Aguardar thread terminar
                if (receiveThread && receiveThread->joinable()) {
                        receiveThread->join();
                }

                // Socket fecha automaticamente via RAII
        }

        bool connect() {
                int sock = socket(AF_INET, SOCK_STREAM, 0);
                clientSocket = std::make_unique<SocketGuard>(sock);

                if (!clientSocket->is_valid()) {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cerr << "Erro ao criar socket" << std::endl;
                        return false;
                }

                sockaddr_in serverAddr{};
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(serverPort);
                inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

                if (::connect(clientSocket->get(), (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cerr << "Erro ao conectar ao servidor" << std::endl;
                        return false;
                }

                {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "Conectado ao servidor " << serverIP << ":" << serverPort << std::endl;
                }

                running = true;

                // Criar thread com smart pointer
                receiveThread = std::make_unique<std::thread>(&TCPChatClient::receiveMessages, this);

                return true;
        }

        void receiveMessages() {
                std::string acc;
                char buf[1024];

                while (running) {
                        int n = recv(clientSocket->get(), buf, sizeof(buf), 0);
                        if (n <= 0) {
                                {
                                        std::lock_guard<std::mutex> lock(coutMutex);
                                        std::cout << "\nDesconectado do servidor" << std::endl;
                                }
                                running = false;
                                kill(getpid(), SIGTERM);
                        }

                        acc.append(buf, n);

                        size_t pos;
                        while ((pos = acc.find('\n')) != std::string::npos) {
                                std::string line = acc.substr(0, pos);
                                acc.erase(0, pos + 1);

                                if (!line.empty() && line.back() == '\r') {
                                        line.pop_back();
                                }

                                if (!line.empty()) {
                                        std::lock_guard<std::mutex> lock(coutMutex);
                                        std::cout << "\r" << std::string(50, ' ') << "\r";
                                        std::cout << line << std::endl;
                                        std::cout << "> " << std::flush;
                                }
                        }
                }
        }

        void sendMessage(const std::string& message) {
                std::string out = message;
                if (out.empty() || out.back() != '\n') {
                        out.push_back('\n');
                }
                send(clientSocket->get(), out.data(), out.size(), 0);
        }

        void start() {
                if (!connect())
                        return;

                std::string message;
                bool firstMessage = true;

                {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "\n=== Chat TCP ===" << std::endl;
                        std::cout << "Digite suas mensagens abaixo." << std::endl;
                        std::cout << "Comando 'sair' para encerrar" << std::endl;
                        std::cout << "================\n"
                                  << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                while (running) {
                        if (firstMessage) {
                                firstMessage = false;
                        } else {
                                std::lock_guard<std::mutex> lock(coutMutex);
                                std::cout << "> " << std::flush;
                        }
                        if (running)
                                std::getline(std::cin, message);

                        if (!running)
                                break;

                        if (message == "sair" || message == "exit" || message == "quit") {
                                {
                                        std::lock_guard<std::mutex> lock(coutMutex);
                                        std::cout << "Encerrando..." << std::endl;
                                }
                                running = false;
                                sendMessage("DISCONNECT");
                                break;
                        }

                        if (!message.empty()) {
                                sendMessage(message);
                        }
                }

                // Aguardar um pouco para mensagens pendentes
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Socket e thread fecham automaticamente (RAII via destrutor)
        }
};

int main(int argc, char* argv[]) {
        try {
                std::string serverIP = "127.0.0.1";
                int serverPort = 8080;

                if (argc >= 2)
                        serverIP = argv[1];
                if (argc >= 3)
                        serverPort = std::stoi(argv[2]);

                TCPChatClient client(serverIP, serverPort);
                client.start();

        } catch (const std::exception& e) {
                std::cerr << "Erro fatal: " << e.what() << std::endl;
                return 1;
        }

        return 0;
}
