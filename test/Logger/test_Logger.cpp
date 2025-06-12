#include <ArduinoFake.h>
#include <unity.h>
#include "Logger.h"
#include "mock_helpers.h"

using namespace fakeit;


void test_logger_levels() {
    std::cout << "\n=== Starting test_logger_levels() ===" << std::endl;

    Stream* streamPtr = ArduinoFakeMock(Stream);
    char buffer[256];
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            return strlen(str);
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            strcat(buffer, "\n");
            return strlen(str) + 1;
        });
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
    
    Logger logger(streamPtr, "[Test]");
    
    // Set global log level to DEBUG (show all messages)
    Logger::s_logLevel = LogLevel::DEBUG;
    logger.setLogLevel(LogLevel::DEBUG);
    
    // Test DEBUG level
    buffer[0] = '\0';
    logger.debug("Debug message");
    TEST_ASSERT_MESSAGE(strstr(buffer, "DEBUG") != nullptr, "DEBUG level not found in output");
    TEST_ASSERT_MESSAGE(strstr(buffer, "Debug message") != nullptr, "Debug message not found in output");
    
    // Test INFO level
    buffer[0] = '\0';
    logger.info("Info message");
    TEST_ASSERT_MESSAGE(strstr(buffer, "INFO") != nullptr, "INFO level not found in output");
    TEST_ASSERT_MESSAGE(strstr(buffer, "Info message") != nullptr, "Info message not found in output");
    
    // Test WARNING level
    buffer[0] = '\0';
    logger.warning("Warning message");
    TEST_ASSERT_MESSAGE(strstr(buffer, "WARN") != nullptr, "WARN level not found in output");
    TEST_ASSERT_MESSAGE(strstr(buffer, "Warning message") != nullptr, "Warning message not found in output");
    
    // Test ERROR level
    buffer[0] = '\0';
    logger.error("Error message");
    TEST_ASSERT_MESSAGE(strstr(buffer, "ERROR") != nullptr, "ERROR level not found in output");
    TEST_ASSERT_MESSAGE(strstr(buffer, "Error message") != nullptr, "Error message not found in output");
    
    // Test CRITICAL level
    buffer[0] = '\0';
    logger.critical("Critical message");
    TEST_ASSERT_MESSAGE(strstr(buffer, "CRIT") != nullptr, "CRIT level not found in output");
    TEST_ASSERT_MESSAGE(strstr(buffer, "Critical message") != nullptr, "Critical message not found in output");
}

void test_logger_level_filtering() {
    std::cout << "  Running test_logger_level_filtering()" << std::endl;
    Stream* streamPtr = ArduinoFakeMock(Stream);
    char buffer[256];
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            return strlen(str);
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            strcat(buffer, "\n");
            return strlen(str) + 1;
        });
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
    
    Logger logger(streamPtr, "[Test]");
    
    // Set global log level to WARNING (only WARNING and above should be logged)
    Logger::s_logLevel = LogLevel::WARNING;
    logger.setLogLevel(LogLevel::WARNING);
    
    // These should not be logged
    buffer[0] = '\0';
    logger.debug("Debug message");
    logger.info("Info message");
    TEST_ASSERT_EQUAL_STRING("", buffer);
    
    // These should be logged
    buffer[0] = '\0';
    logger.warning("Warning message");
    TEST_ASSERT(strstr(buffer, "WARN") != nullptr);
    
    buffer[0] = '\0';
    logger.error("Error message");
    TEST_ASSERT(strstr(buffer, "ERROR") != nullptr);
    
    buffer[0] = '\0';
    logger.critical("Critical message");
    TEST_ASSERT(strstr(buffer, "CRIT") != nullptr);
}

void test_logger_raw_output() {
    std::cout << "  Running test_logger_raw_output()" << std::endl;
    
    Stream* streamPtr = ArduinoFakeMock(Stream);
    char buffer[256];
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            return strlen(str);
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([&buffer](const char* str) {
            strcat(buffer, str);
            strcat(buffer, "\n");
            return strlen(str) + 1;
        });
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
    Logger logger(streamPtr, "[Test]");
    
    buffer[0] = '\0';
    logger.raw("Raw message");
    TEST_ASSERT_EQUAL_STRING("Raw message\n", buffer);
}

void runLoggerTests() {
    UNITY_BEGIN();
    RUN_TEST(test_logger_levels);
    RUN_TEST(test_logger_level_filtering);
    RUN_TEST(test_logger_raw_output);
    UNITY_END();
}
