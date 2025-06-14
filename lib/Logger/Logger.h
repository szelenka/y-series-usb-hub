/**
 * @file Logger.h
 * @brief Lightweight logging utility for the Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file defines the Logger class which provides a flexible and efficient
 * logging solution for Arduino-based projects. It supports multiple log levels,
 * custom output streams, and formatted message output.
 *
 * The Logger is responsible for:
 * - Supporting multiple log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
 * - Outputting to any Stream-compatible interface (Serial, SoftwareSerial, etc.)
 * - Providing formatted string output with variable arguments
 * - Allowing per-instance log level filtering
 */

#ifndef Y_SERIES_USB_HUB_LOGGER_H
#define Y_SERIES_USB_HUB_LOGGER_H

// System includes
#include <Arduino.h>

/**
 * @brief Enumerates the severity levels for log messages
 *
 * This enum class defines the different levels of logging severity,
 * from detailed debugging information to critical errors that may
 * prevent normal operation.
 */
enum class LogLevel : uint8_t
{
    DEBUG = 0,     ///< Detailed debug information for development
    INFO = 1,      ///< General operational messages
    WARNING = 2,   ///< Indicates potential issues that don't prevent operation
    ERROR = 3,     ///< Error conditions that might affect functionality
    CRITICAL = 4,  ///< Critical conditions that prevent normal operation
    NONE = 5       ///< No logging (turns off all output)
};

/**
 * @brief Provides flexible logging functionality with multiple severity levels
 *
 * @details
 * The Logger class implements a thread-safe logging mechanism that supports
 * multiple output streams and configurable log levels. It's designed to be
 * lightweight enough for embedded systems while providing rich functionality.
 *
 * Features:
 * - Multiple log levels with compile-time filtering
 * - Support for any Stream-compatible output
 * - Formatted string output (printf-style)
 * - Optional message prefixing
 * - Global instance for convenience
 */
class Logger
{
public:
    /// @name Construction and Assignment
    /// @{

    /**
     * @brief Construct a new Logger instance
     *
     * @param[in] serial Pointer to the Stream object for output (e.g., &Serial)
     * @param[in] prefix Optional prefix for all log messages (default: empty)
     *
     * @note The Stream object must remain valid for the lifetime of the Logger
     * @warning Passing a null Stream pointer will result in no output
     */
    explicit Logger(Stream* serial, const char* prefix = "");

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Allow moving
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;

    /// @}

    /// @name Logging Methods
    /// @{

    /**
     * @brief Log a debug message
     *
     * @param[in] format Format string (printf-style)
     * @param[in] ... Arguments for the format string
     *
     * @note Only logged if current log level is DEBUG or lower
     */
    void debug(const char* format, ...) const;

    /**
     * @brief Log an informational message
     *
     * @param[in] format Format string (printf-style)
     * @param[in] ... Arguments for the format string
     *
     * @note Only logged if current log level is INFO or lower
     */
    void info(const char* format, ...) const;

    /**
     * @brief Log a warning message
     *
     * @param[in] format Format string (printf-style)
     * @param[in] ... Arguments for the format string
     *
     * @note Only logged if current log level is WARNING or lower
     */
    void warning(const char* format, ...) const;

    /**
     * @brief Log an error message
     *
     * @param[in] format Format string (printf-style)
     * @param[in] ... Arguments for the format string
     *
     * @note Only logged if current log level is ERROR or lower
     */
    void error(const char* format, ...) const;

    /**
     * @brief Log a critical error message
     *
     * @param[in] format Format string (printf-style)
     * @param[in] ... Arguments for the format string
     *
     * @note Always logged unless log level is set to NONE
     */
    void critical(const char* format, ...) const;

    /**
     * @brief Output a raw message without any formatting
     *
     * @param[in] message The message to output
     *
     * @note Not affected by log level settings
     */
    void raw(const char* message) const;

    /// @}

    /// @name Log Level Management
    /// @{

    /**
     * @brief Set the minimum log level for this logger
     *
     * @param[in] level Minimum level to log (messages below this level are filtered)
     */
    void setLogLevel(LogLevel level) { m_logLevel = level; }

    /**
     * @brief Get the current log level
     *
     * @return LogLevel Current minimum log level
     */
    LogLevel getLogLevel() const { return m_logLevel; }

    /**
     * @brief Check if a message with the given level would be logged
     *
     * @param[in] level The log level to check
     * @return true If a message with this level would be logged
     * @return false If the message would be filtered out
     */
    bool isLoggable(LogLevel level) const
    {
        return m_serial != nullptr &&
               static_cast<uint8_t>(level) >= static_cast<uint8_t>(m_logLevel);
    }

    /// @}

private:
    /// @name Internal Implementation
    /// @{

    /**
     * @brief Internal method to handle the actual logging
     *
     * @param[in] level Severity level of the message
     * @param[in] format Format string (printf-style)
     * @param[in] args Variable arguments for the format string
     */
    void log(LogLevel level, const char* format, va_list args) const;

    /**
     * @brief Convert a LogLevel to its string representation
     *
     * @param[in] level The log level to convert
     * @return const char* String representation of the level
     */
    static const char* levelToString(LogLevel level);

    /// @}

    /// @name Member Variables
    /// @{
    Stream* m_serial;      ///< Output stream for log messages
    const char* m_prefix;  ///< Optional prefix for all messages
    LogLevel m_logLevel;   ///< Current minimum log level
    /// @}
};

/**
 * @brief Global Logger instance for convenience
 *
 * @note This is the primary logger instance used throughout the application.
 *       It's initialized to use the default Serial port with no prefix.
 */
extern Logger Log;

#endif  // Y_SERIES_USB_HUB_LOGGER_H
