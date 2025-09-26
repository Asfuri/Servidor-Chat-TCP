#include <iostream>
#include <string>
#include <thread>
#ifndef LOGENTRY_H
#define LOGENTRY_H
class logEntry {
public:
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        std::string message;
};
#endif