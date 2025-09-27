#include <iostream>
#include <string>
#include <thread>
#ifndef LOGENTRY_H
#define LOGENTRY_H
<<<<<<< Updated upstream
class logEntry {
=======
class LogEntry {
>>>>>>> Stashed changes
public:
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        std::string message;
};
#endif