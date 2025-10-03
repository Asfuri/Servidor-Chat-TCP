#include <arpa/inet.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

// Mutex global para proteger std::cout
static std::mutex coutMutex;

class TCPChatClient {
private:
        int clientSocket;
        std::string serverIP;
        int serverPort;

public:
        TCPChatClient(const std::string &ip = "127.0.0.1", int port = 8080)
            : serverIP(ip), serverPort(port) {
        }

        bool connect() {
                clientSocket = socket(AF_INET, SOCK_STREAM, 0);

                sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(serverPort);
                inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

                if (::connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) <
                    0) {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cerr << "Erro ao conectar ao servidor" << std::endl;
                        return false;
                }

                {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "Conectado ao servidor " << serverIP << ":" << serverPort
                                  << std::endl;
                }

                // Iniciar thread para receber mensagens
                std::thread receiveThread(&TCPChatClient::receiveMessages, this);
                receiveThread.detach();

                return true;
        }

        void receiveMessages() {
                std::string acc;
                char buf[1024];
                while (true) {
                        int n = recv(clientSocket, buf, sizeof(buf), 0);
                        if (n <= 0) {
                                std::lock_guard<std::mutex> lock(coutMutex);
                                std::cout << "Desconectado do servidor" << std::endl;
                                break;
                        }
                        acc.append(buf, n);

                        size_t pos;
                        while ((pos = acc.find('\n')) != std::string::npos) {
                                std::string line = acc.substr(0, pos);
                                acc.erase(0, pos + 1);

                                // opcional: remover \r
                                if (!line.empty() && line.back() == '\r')
                                        line.pop_back();

                                // evita imprimir linhas vazias quando há \n\n
                                if (!line.empty()) {
                                        std::lock_guard<std::mutex> lock(coutMutex);
                                        std::cout << line << std::endl;
                                }
                        }
                }
        }

        void sendMessage(const std::string &message) {
                std::string out = message;
                if (out.empty() || out.back() != '\n')
                        out.push_back('\n');
                send(clientSocket, out.data(), out.size(), 0);
        }

        void start() {
                if (!connect())
                        return;

                std::string message;
                {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "Digite mensagens (ou 'sair' para encerrar):" << std::endl;
                }

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

int main(int argc, char *argv[]) {
        std::string serverIP = "127.0.0.1";
        int serverPort = 8080;

        if (argc >= 2)
                serverIP = argv[1];
        if (argc >= 3)
                serverPort = std::stoi(argv[2]);

        TCPChatClient client(serverIP, serverPort);
        client.start();

        return 0;
}
