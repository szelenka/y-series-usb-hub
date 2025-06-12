#include <ArduinoFake.h>
#include <unity.h>

#include "Animation.h"
#include "mock_helpers.h"

void test_read_inputs_from_pins()
{
    std::cout << "  Running test_read_inputs_from_pins()" << std::endl;

    // Create mock objects
    const AnimationPins pins = AnimationPins();

    // Mock Arduino functions
    When(Method(ArduinoFake(), digitalRead))
        .AlwaysDo(
            [](uint8_t pin)
            {
                return HIGH;  // All pins are high
            });
    When(Method(ArduinoFake(), millis)).Do([]() { return 1000; });

    // Read inputs from pins
    std::cout << "Reading inputs from pins..." << std::endl;
    AnimationInputs inputs = readInputs(pins);

    // Verify inputs were read correctly
    std::cout << "Verifying inputs were read correctly..." << std::endl;
    TEST_ASSERT_EQUAL(HIGH, inputs.sensorLeft);
    TEST_ASSERT_EQUAL(HIGH, inputs.sensorRight);
    TEST_ASSERT_EQUAL(HIGH, inputs.pirSensor);
    TEST_ASSERT_EQUAL(HIGH, inputs.buttonRectangle);
    TEST_ASSERT_EQUAL(HIGH, inputs.buttonCircle);
    TEST_ASSERT_EQUAL(1000, inputs.currentTime);
}

void runAnimationInputsTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_read_inputs_from_pins);
    UNITY_END();
}
