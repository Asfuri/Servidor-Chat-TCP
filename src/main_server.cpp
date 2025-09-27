#include "../lib/libtslog.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

int main() {
<<<<<<< Updated upstream
        libtslog logger;
        logger.initialize("chat_server.log");
=======
        ThreadSafeLogger logger;
        logger.initialize("../chat_server.log");
>>>>>>> Stashed changes

        // Simula múltiplas threads fazendo log
        std::vector<std::thread> threads;

        for (int i = 0; i < 5; i++) {
                threads.emplace_back([&logger, i]() {
                        for (int j = 0; j < 100; j++) {
                                logger.log("Thread " + std::to_string(i) + " - Mensagem " + std::to_string(j));
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        }
                });
        }

        // Aguarda todas as threads
        for (auto& t : threads) {
                t.join();
        }

        logger.shutdown(); // Garante que todos os logs foram escritos
}