#include "../lib/libtslog.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

static ThreadSafeLogger logger;
class TCPChatClient {
private:
        int clientSocket;
        std::string serverIP;
        int serverPort;

public:
        TCPChatClient(const std::string& ip = "127.0.0.1", int port = 8080) : serverIP(ip), serverPort(port) {
        }

        bool connect() {
                clientSocket = socket(AF_INET, SOCK_STREAM, 0);

                sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(serverPort);
                inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

                if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                        std::cerr << "Erro ao conectar ao servidor" << std::endl;
                        logger.log("Erro ao conectar ao servidor: " + std::string(strerror(errno)));
                        return false;
                }
                logger.log("Conectado ao servidor " + serverIP + ":" + std::to_string(serverPort));
                std::cout << "Conectado ao servidor " << serverIP << ":" << serverPort << std::endl;

                // Iniciar thread para receber mensagens
                std::thread receiveThread(&TCPChatClient::receiveMessages, this);
                receiveThread.detach();

                return true;
        }

        void receiveMessages() {
                char buffer[1024];

                while (true) {
                        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

                        if (bytesRead <= 0) {
                                std::cout << "Desconectado do servidor" << std::endl;
                                break;
                        }

                        buffer[bytesRead] = '\0';
                        logger.log("Mensagem recebida: " + std::string(buffer));
                        std::cout << buffer << std::endl;
                }
        }

        void sendMessage(const std::string& message) {
                send(clientSocket, message.c_str(), message.length(), 0);
                logger.log("Enviado ao servidor: " + message);
        }

        void start() {
                if (!connect())
                        return;

                std::string message;
                std::cout << "Digite mensagens (ou 'sair' para encerrar):" << std::endl;

                while (true) {
                        std::getline(std::cin, message);

                        if (message == "sair") {
                                break;
                        }

                        sendMessage(message);
                }

                close(clientSocket);
        }
};

int main(int argc, char* argv[]) {
        logger.initialize("logs/client.log");
        logger.log("Cliente iniciando");
        std::string serverIP = "127.0.0.1";
        int serverPort = 8080;

        if (argc >= 2)
                serverIP = argv[1];
        if (argc >= 3)
                serverPort = std::stoi(argv[2]);

        TCPChatClient client(serverIP, serverPort);
        client.start();
        logger.log("Cliente encerrando");
        logger.shutdown();
}
