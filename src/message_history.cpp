#include "../lib/message_history.h"
#include <iomanip>
#include <sstream>

MessageHistory::MessageHistory(size_t max) : maxSize(max) {
}

void MessageHistory::addMessage(const std::string& msg, int senderSocket) {
        std::lock_guard<std::mutex> lock(historyMutex);

        HistoryEntry entry;
        entry.message = msg;
        entry.timestamp = std::chrono::system_clock::now();
        entry.senderSocket = senderSocket;

        messages.push_back(entry);

        // Limitar tamanho do histórico
        if (messages.size() > maxSize) {
                messages.pop_front();
        }
}

std::vector<std::string> MessageHistory::getRecentMessages(size_t count) const {
        std::lock_guard<std::mutex> lock(historyMutex);

        std::vector<std::string> result;

        size_t start = messages.size() > count ? messages.size() - count : 0;

        for (size_t i = start; i < messages.size(); ++i) {
                // Formatar: [HH:MM:SS] Cliente X: mensagem
                auto time = std::chrono::system_clock::to_time_t(messages[i].timestamp);
                std::tm tm = *std::localtime(&time);

                std::ostringstream oss;
                oss << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << messages[i].message;

                result.push_back(oss.str());
        }

        return result;
}

std::vector<std::string> MessageHistory::getAllMessages() const {
        return getRecentMessages(messages.size());
}

size_t MessageHistory::size() const {
        std::lock_guard<std::mutex> lock(historyMutex);
        return messages.size();
}

void MessageHistory::clear() {
        std::lock_guard<std::mutex> lock(historyMutex);
        messages.clear();
}
