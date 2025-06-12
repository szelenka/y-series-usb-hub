/**
 * @file Logger.cpp
 * @brief Implementation of the Logger class
 */

#include "Logger.h"
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <Arduino.h>

using namespace std;

// Initialize static members
#ifdef NDEBUG
LogLevel Logger::s_logLevel = LogLevel::INFO;
#else
LogLevel Logger::s_logLevel = LogLevel::DEBUG;
#endif

// Global logger instance
Logger Log(&Serial);

// Maximum length of a single log message (including prefix, level, etc.)
constexpr size_t MAX_LOG_LENGTH = 128;

// ANSI color codes for different log levels (if supported)
#ifdef LOGGER_USE_ANSI_COLORS
const char* ANSI_COLOR_RESET = "\033[0m";
const char* ANSI_COLOR_RED = "\033[31m";
const char* ANSI_COLOR_GREEN = "\033[32m";
const char* ANSI_COLOR_YELLOW = "\033[33m";
const char* ANSI_COLOR_BLUE = "\033[34m";
const char* ANSI_COLOR_MAGENTA = "\033[35m";
#else
const char* ANSI_COLOR_RESET = "";
const char* ANSI_COLOR_RED = "";
const char* ANSI_COLOR_GREEN = "";
const char* ANSI_COLOR_YELLOW = "";
const char* ANSI_COLOR_BLUE = "";
const char* ANSI_COLOR_MAGENTA = "";
#endif

void Logger::log(LogLevel level, const char* format, va_list args) const {
    // Debug: Log entry
    if (!isLoggable(level)) {
        return;
    }
    // Format the timestamp if needed
    char timestamp[16] = {0};
    if (m_serial != &Serial) {  // Only add timestamp if not using Serial
        unsigned long now = millis();
        snprintf(timestamp, sizeof(timestamp), "%10lu ", now);
    }

    // Make a copy of args since vsnprintf consumes it
    va_list args_copy;
    va_copy(args_copy, args);
    
    // Format the message
    char message[MAX_LOG_LENGTH] = {0};
    int written = vsnprintf(message, sizeof(message), format, args);
    
    // Debug: Check for buffer overflow
    if (written >= static_cast<int>(sizeof(message))) {
        cout << "[WARNING] Logger::log - Message truncated" << endl;
    }

    // Actual log output
    if (m_serial) {
        m_serial->print(timestamp);
        m_serial->print("[");
        m_serial->print(levelToString(level));
        m_serial->print("] ");
        
        if (m_prefix && m_prefix[0] != '\0') {
            m_serial->print(m_prefix);
            m_serial->print(" ");
        }
        
        m_serial->println(message);
    }
    
    // Clean up
    va_end(args_copy);
}

void Logger::debug(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, format, args);
    va_end(args);
}

void Logger::info(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    log(LogLevel::INFO, format, args);
    va_end(args);
}

void Logger::warning(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    log(LogLevel::WARNING, format, args);
    va_end(args);
}

void Logger::error(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    log(LogLevel::ERROR, format, args);
    va_end(args);
}

void Logger::critical(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    log(LogLevel::CRITICAL, format, args);
    va_end(args);
}

void Logger::raw(const char* message) const {
    if (m_serial) {
        m_serial->println(message);
    }
}

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::CRITICAL:return "CRIT ";
        default:                return "?????";
    }
}
