#ifndef MESSAGE_HISTORY_H
#define MESSAGE_HISTORY_H

#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

struct HistoryEntry {
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        int senderSocket;
};

class MessageHistory {
private:
        std::deque<HistoryEntry> messages;
        mutable std::mutex historyMutex; // mutable para uso em métodos const
        const size_t maxSize;

public:
        explicit MessageHistory(size_t max = 100);

        // Adiciona mensagem ao histórico
        void addMessage(const std::string& msg, int senderSocket);

        // Retorna últimas N mensagens
        std::vector<std::string> getRecentMessages(size_t count = 10) const;

        // Retorna todas as mensagens
        std::vector<std::string> getAllMessages() const;

        // Retorna total de mensagens no histórico
        size_t size() const;

        // Limpa todo o histórico
        void clear();
};

#endif // MESSAGE_HISTORY_H
