#include <ArduinoFake.h>
#include <unity.h>
#include "Logger.h"
#include "mock_helpers.h"

using namespace fakeit;

void test_logger_enabled()
{
    std::cout << "  Running test_logger_enabled()" << std::endl;
    Stream* streamPtr = ArduinoFakeMock(Stream);
    Logger logger(streamPtr, "[Test] ");
    Logger::s_enabled = true;
    
    char buffer[256];
    buffer[0] = '\0';
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .Do([&buffer](const char* str) {
            strcat(buffer, str);
            return strlen(str);
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .Do([&buffer](const char* str) {
            strcat(buffer, str);
            strcat(buffer, "\n");
            return strlen(str) + 1;
        });
    
    // Test logging when enabled
    logger.print("Test message");
    TEST_ASSERT_EQUAL_STRING("[Test] Test message\n", buffer);
    
    // Test logging when disabled
    Logger::s_enabled = false;
    logger.print("Another message");
    TEST_ASSERT_EQUAL_STRING("[Test] Test message\n", buffer);  // Shouldn't have changed
}

void test_logger_prefix()
{
    std::cout << "  Running test_logger_prefix()" << std::endl;
    Stream* streamPtr = ArduinoFakeMock(Stream);
    Logger logger(streamPtr, "[Custom] ");  
    Logger::s_enabled = true;

    char buffer[256];
    buffer[0] = '\0';
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .Do([&buffer](const char* str) {
            strcat(buffer, str);
            return strlen(str);
        });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .Do([&buffer](const char* str) {
            strcat(buffer, str);
            strcat(buffer, "\n");
            return strlen(str) + 1;
        });
    
    logger.print("Test message");
    TEST_ASSERT_EQUAL_STRING("[Custom] Test message\n", buffer);
}

void runLoggerTests() {
    UNITY_BEGIN();
    RUN_TEST(test_logger_enabled);
    RUN_TEST(test_logger_prefix);
    UNITY_END();
}
