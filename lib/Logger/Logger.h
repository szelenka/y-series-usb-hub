/**
 * @file Logger.h
 * @brief A lightweight logging utility for Arduino and embedded systems
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

/**
 * @brief Logging levels for different severity of messages
 */
enum class LogLevel : uint8_t
{
    DEBUG,     ///< Detailed debug information
    INFO,      ///< General operational messages
    WARNING,   ///< Indicates potential issues
    ERROR,     ///< Error conditions that might still allow the application to continue
    CRITICAL,  ///< Critical conditions that prevent normal operation
    NONE       ///< No logging
};

/**
 * @brief A simple logging class that supports different log levels and output streams
 *
 * This class provides a flexible logging solution that can be used across different
 * Arduino projects. It supports different log levels and can output to any Stream
 * compatible output (Serial, SoftwareSerial, etc.).
 */
class Logger
{
public:
    /**
     * @brief Construct a new Logger instance
     *
     * @param serial Pointer to a Stream object for output (e.g., &Serial)
     * @param prefix Optional prefix for log messages (default: empty)
     */
    explicit Logger(Stream* serial, const char* prefix = "")
        : m_serial(serial), m_prefix(prefix), m_logLevel(LogLevel::INFO)
    {
    }

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Allow moving
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;

    /**
     * @brief Log a formatted message with DEBUG level
     *
     * @param format Format string (printf-style)
     * @param ... Format arguments
     */
    void debug(const char* format, ...) const;

    /**
     * @brief Log a formatted message with INFO level
     *
     * @param format Format string (printf-style)
     * @param ... Format arguments
     */
    void info(const char* format, ...) const;

    /**
     * @brief Log a formatted message with WARNING level
     *
     * @param format Format string (printf-style)
     * @param ... Format arguments
     */
    void warning(const char* format, ...) const;

    /**
     * @brief Log a formatted message with ERROR level
     *
     * @param format Format string (printf-style)
     * @param ... Format arguments
     */
    void error(const char* format, ...) const;

    /**
     * @brief Log a formatted message with CRITICAL level
     *
     * @param format Format string (printf-style)
     * @param ... Format arguments
     */
    void critical(const char* format, ...) const;

    /**
     * @brief Log a raw message without any formatting or level prefix
     *
     * @param message The message to log
     */
    void raw(const char* message) const;

    /**
     * @brief Set the log level for this logger instance
     *
     * @param level The minimum log level to output
     */
    void setLogLevel(LogLevel level) { m_logLevel = level; }

    /**
     * @brief Get the current log level for this logger instance
     *
     * @return LogLevel The current log level
     */
    LogLevel getLogLevel() const { return m_logLevel; }

    /**
     * @brief Check if a log level would be output with current settings
     *
     * @param level The log level to check
     * @return true If the level would be logged
     * @return false If the level would be filtered out
     */
    bool isLoggable(LogLevel level) const
    {
        return m_serial != nullptr &&
               static_cast<uint8_t>(level) >= static_cast<uint8_t>(m_logLevel);
    }

private:
    /**
     * @brief Internal method to handle the actual logging
     *
     * @param level The log level
     * @param format Format string (printf-style)
     * @param args Variable arguments for the format string
     */
    void log(LogLevel level, const char* format, va_list args) const;

    /**
     * @brief Get the string representation of a log level
     *
     * @param level The log level
     * @return const char* String representation of the level
     */
    static const char* levelToString(LogLevel level);

    Stream* m_serial;      ///< Output stream for log messages
    const char* m_prefix;  ///< Prefix for log messages
    LogLevel m_logLevel;   ///< Instance-specific log level
};

// Global logger instance for convenience
extern Logger Log;

#endif  // LOGGER_H
