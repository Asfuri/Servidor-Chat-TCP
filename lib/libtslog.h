#include "logEntry.h"
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#ifndef LIBTSLOG_H
#define LIBTSLOG_H
#define MAX_LOG_QUEUE_SIZE 1000

<<<<<<< Updated upstream
class libtslog {
public:
        libtslog();
        virtual ~libtslog();
        void log(const std::string &message);
        void initialize(const std::string &filename);
        void shutdown();
        std::queue<logEntry> logQueue;
=======
class ThreadSafeLogger {
public:
        ThreadSafeLogger();
        virtual ~ThreadSafeLogger();
        void log(const std::string &message);
        void initialize(const std::string &filename);
        void shutdown();
        std::queue<LogEntry> logQueue;
>>>>>>> Stashed changes
        std::mutex logMutex;

private:
        void logWriterFunc();
        std::thread logThread;
        std::ofstream logFile;
        std::condition_variable logCondition;
        std::atomic<bool> running;
};
#endif