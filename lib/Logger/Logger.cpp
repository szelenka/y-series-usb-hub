/**
 * @file Logger.cpp
 * @brief Implementation of the Logger class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file implements the Logger class which provides a flexible and efficient
 * logging solution for Arduino-based projects. It handles all the low-level
 * details of formatting and outputting log messages with various severity levels.
 */

#include "Logger.h"

// System includes
#include <Arduino.h>

// Standard library includes
#include <cstdarg>
#include <cstdio>
#include <cstring>

// Constants
namespace
{
/// Maximum length of a single log message (including prefix, level, etc.)
constexpr size_t MAX_LOG_LENGTH = 128;

/// Size of timestamp buffer (format: "[HH:MM:SS] ")
constexpr size_t TIMESTAMP_BUFFER_SIZE = 16;

// ANSI color codes for different log levels (if enabled)
#ifdef LOGGER_USE_ANSI_COLORS
constexpr const char* ANSI_COLOR_RESET = "\033[0m";
constexpr const char* ANSI_COLOR_DEBUG = "\033[36m";   // Cyan
constexpr const char* ANSI_COLOR_INFO = "\033[32m";    // Green
constexpr const char* ANSI_COLOR_WARN = "\033[33m";    // Yellow
constexpr const char* ANSI_COLOR_ERROR = "\033[31m";   // Red
constexpr const char* ANSI_COLOR_CRIT = "\033[35;1m";  // Bright Magenta
#else
constexpr const char* ANSI_COLOR_RESET = "";
constexpr const char* ANSI_COLOR_DEBUG = "";
constexpr const char* ANSI_COLOR_INFO = "";
constexpr const char* ANSI_COLOR_WARN = "";
constexpr const char* ANSI_COLOR_ERROR = "";
constexpr const char* ANSI_COLOR_CRIT = "";
#endif
}  // namespace

// Global logger instance (defaults to Serial output)
Logger Log(&Serial);

/**
 * @brief Construct a new Logger instance
 *
 * @param[in] serial Pointer to the Stream object for output
 * @param[in] prefix Optional prefix for all log messages
 */
Logger::Logger(Stream* serial, const char* prefix)
    : m_serial(serial), m_prefix(prefix), m_logLevel(LogLevel::INFO)
{
    // No initialization needed beyond member initializers
}

/**
 * @brief Internal method to handle the actual logging
 *
 * @param[in] level Severity level of the message
 * @param[in] format Format string (printf-style)
 * @param[in] args Variable arguments for the format string
 */
void Logger::log(LogLevel level, const char* format, va_list args) const
{
    // Early return if logging is disabled or level is not sufficient
    if (!isLoggable(level) || !m_serial)
    {
        return;
    }

    // Format timestamp if needed (only for non-Serial outputs)
    char timestamp[TIMESTAMP_BUFFER_SIZE] = {0};
    if (m_serial != &Serial)
    {
        const unsigned long now = millis();
        snprintf(timestamp, sizeof(timestamp), "%10lu ", now);
    }

    // Format the message
    char message[MAX_LOG_LENGTH] = {0};
    va_list args_copy;
    va_copy(args_copy, args);
    vsnprintf(message, sizeof(message), format, args_copy);
    va_end(args_copy);

    // Output the log message with appropriate formatting
    m_serial->print(timestamp);

    // Add color if enabled
#ifdef LOGGER_USE_ANSI_COLORS
    switch (level)
    {
        case LogLevel::DEBUG:
            m_serial->print(ANSI_COLOR_DEBUG);
            break;
        case LogLevel::INFO:
            m_serial->print(ANSI_COLOR_INFO);
            break;
        case LogLevel::WARNING:
            m_serial->print(ANSI_COLOR_WARN);
            break;
        case LogLevel::ERROR:
            m_serial->print(ANSI_COLOR_ERROR);
            break;
        case LogLevel::CRITICAL:
            m_serial->print(ANSI_COLOR_CRIT);
            break;
        default:
            break;
    }
#endif

    m_serial->print("[");
    m_serial->print(levelToString(level));
    m_serial->print("] ");

    // Add prefix if specified
    if (m_prefix && m_prefix[0] != '\0')
    {
        m_serial->print(m_prefix);
        m_serial->print(" ");
    }

    // Print the actual message and reset color
    m_serial->print(message);
    m_serial->print(ANSI_COLOR_RESET);
    m_serial->println();
}

// Logging methods for each log level
void Logger::debug(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, format, args);
    va_end(args);
}

void Logger::info(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    log(LogLevel::INFO, format, args);
    va_end(args);
}

void Logger::warning(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    log(LogLevel::WARNING, format, args);
    va_end(args);
}

void Logger::error(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    log(LogLevel::ERROR, format, args);
    va_end(args);
}

void Logger::critical(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    log(LogLevel::CRITICAL, format, args);
    va_end(args);
}

/**
 * @brief Output a raw message without any formatting
 *
 * @param[in] message The message to output
 */
void Logger::raw(const char* message) const
{
    if (m_serial)
    {
        m_serial->println(message);
    }
}

/**
 * @brief Convert a LogLevel to its string representation
 *
 * @param[in] level The log level to convert
 * @return const char* String representation of the level
 */
const char* Logger::levelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO ";
        case LogLevel::WARNING:
            return "WARN ";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRIT ";
        case LogLevel::NONE:
            return "NONE ";
        default:
            return "?????";
    }
}
