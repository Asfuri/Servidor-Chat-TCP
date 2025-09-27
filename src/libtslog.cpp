#include "../lib/libtslog.h"
#include "../lib/logEntry.h"
#include <iostream>

ThreadSafeLogger::ThreadSafeLogger() : running(false) {
}

ThreadSafeLogger::~ThreadSafeLogger() {
        shutdown();
}

void ThreadSafeLogger::initialize(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
                std::cerr << "Failed to open log file: " << filename << std::endl;
                return;
        }
        running = true;
        logThread = std::thread(&ThreadSafeLogger::logWriterFunc, this);
}

void ThreadSafeLogger::log(const std::string& message) {
        LogEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = std::this_thread::get_id();
        entry.message = message;

        std::unique_lock<std::mutex> lock(logMutex);
        if (logQueue.size() >= MAX_LOG_QUEUE_SIZE) {
                logQueue.pop();
        }
        logQueue.push(entry);
        logCondition.notify_one();
}

void ThreadSafeLogger::logWriterFunc() {
        while (running || !logQueue.empty()) {
                std::unique_lock<std::mutex> lock(logMutex);
                logCondition.wait(lock, [this] { return !logQueue.empty() || !running; });

                while (!logQueue.empty()) {
                        LogEntry entry = logQueue.front();
                        logQueue.pop();
                        lock.unlock();

                        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
                        logFile << std::ctime(&time) << " [Thread " << entry.threadId << "] " << entry.message << std::endl;

                        lock.lock();
                }
        }
}

void ThreadSafeLogger::shutdown() {
        if (!running) return;
        running = false;
        logCondition.notify_all();

        if (logThread.joinable()) {
                logThread.join();
        }
        if (logFile.is_open()) {
                logFile.close();
        }
}