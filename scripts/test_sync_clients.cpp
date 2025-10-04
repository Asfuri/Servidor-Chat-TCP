#include <arpa/inet.h>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

// Mutex global para sincronizar saída
std::mutex cout_mutex;

// Barreira para sincronização de threads
class Barrier {
private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
        int threshold;

public:
        explicit Barrier(int num_threads) : count(0), threshold(num_threads) {
        }

        void wait() {
                std::unique_lock<std::mutex> lock(mtx);
                count++;

                if (count >= threshold) {
                        // Última thread chegou - libera todas
                        cv.notify_all();
                } else {
                        // Aguarda até todas chegarem
                        cv.wait(lock, [this]() { return count >= threshold; });
                }
        }

        // Reset para reutilizar a barreira
        void reset() {
                std::unique_lock<std::mutex> lock(mtx);
                count = 0;
        }
};

// Conecta ao servidor e aguarda barreira antes de enviar
void clientThread(int id, Barrier& startBarrier, Barrier& endBarrier) {
        // 1. Conectar ao servidor
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cerr << "Cliente " << id << ": Falha na conexão" << std::endl;
                return;
        }

        {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "✅ Cliente " << id << ": Conectado" << std::endl;
        }

        // 2. **SINCRONIZAÇÃO INICIAL**: Aguardar todos conectarem
        startBarrier.wait();

        {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "🚀 Cliente " << id << ": Enviando mensagem" << std::endl;
        }

        // 3. Enviar mensagem
        std::string msg = "Mensagem do cliente " + std::to_string(id) + "\n";
        send(sock, msg.c_str(), msg.length(), 0);

        // 4. Aguardar resposta (histórico + broadcasts)
        char buffer[1024];
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
                buffer[n] = '\0';
                std::string received(buffer);

                {
                        std::lock_guard<std::mutex> lock(cout_mutex);
                        std::cout << "📥 Cliente " << id << " recebeu:\n";

                        // Separar linhas e mostrar cada mensagem
                        std::istringstream iss(received);
                        std::string line;
                        while (std::getline(iss, line)) {
                                if (!line.empty()) {
                                        if (line.find("===") != std::string::npos) {
                                                // Linha de cabeçalho
                                                std::cout << "   " << line << std::endl;
                                        } else if (line.find("Cliente") != std::string::npos) {
                                                // Mensagem de chat
                                                std::cout << "   💬 " << line << std::endl;
                                        } else {
                                                // Outras linhas
                                                std::cout << "   " << line << std::endl;
                                        }
                                }
                        }
                        std::cout << std::endl;
                }
        }

        // Pequeno delay para dar tempo do servidor processar
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 5. **SINCRONIZAÇÃO FINAL**: Aguardar todos terminarem antes de desconectar
        endBarrier.wait();

        // 6. Desconectar
        std::string sair = "sair\n";
        send(sock, sair.c_str(), sair.length(), 0);
        close(sock);

        {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "🔴 Cliente " << id << ": Desconectado" << std::endl;
        }
}

int main(int argc, char* argv[]) {
        int numClients = 3;
        if (argc > 1) {
                numClients = std::stoi(argv[1]);
        }

        std::cout << "🧪 Teste de Sincronização com " << numClients << " clientes\n"
                  << std::endl;

        // Criar duas barreiras: uma para início (conexões) e outra para fim (desconexões)
        Barrier startBarrier(numClients);
        Barrier endBarrier(numClients);

        // Criar threads de clientes
        std::vector<std::thread> threads;
        for (int i = 1; i <= numClients; i++) {
                threads.emplace_back(clientThread, i, std::ref(startBarrier), std::ref(endBarrier));
        }

        // Aguardar todas terminarem - SEM SLEEP!
        for (auto& t : threads) {
                t.join();
        }

        std::cout << "\n✅ Teste concluído!" << std::endl;
        return 0;
}
