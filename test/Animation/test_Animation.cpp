#include <ArduinoFake.h>
#include <unity.h>

#include "Animation.h"
#include "EyeAnimation.h"
#include "mock_helpers.h"

using namespace fakeit;

void test_animation_initialization()
{
    std::cout << "  Running test_animation_initialization()" << std::endl;

    // Create mock objects
    Mock<EyeAnimation> pixelsMock;
    Mock<AudioPlayer> audioPlayerMock;

    // Create animation object
    Animation* animation =
        new Animation(&pixelsMock.get(), &audioPlayerMock.get(), AnimationPins());

    // Verify initialization
    TEST_ASSERT_NOT_NULL(animation);

    // Clean up
    delete animation;
}

void test_read_inputs()
{
    std::cout << "  Running test_read_inputs()" << std::endl;

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

    // Create animation object
    Animation* animation = new Animation(nullptr, nullptr, pins);

    // Set inputs using setter methods
    std::cout << "Setting inputs using setter methods..." << std::endl;
    animation->setInputSensorLeft(HIGH);
    animation->setInputSensorRight(LOW);
    animation->setInputPIRSensor(LOW);
    animation->setInputButtonRectangle(LOW);
    animation->setInputButtonCircle(LOW);
    animation->setCurrentTime(1000);

    // Verify inputs were set correctly
    std::cout << "Verifying inputs were set correctly..." << std::endl;
    TEST_ASSERT_EQUAL(HIGH, animation->getInputSensorLeft());
    TEST_ASSERT_EQUAL(LOW, animation->getInputSensorRight());
    TEST_ASSERT_EQUAL(LOW, animation->getInputPIRSensor());
    TEST_ASSERT_EQUAL(LOW, animation->getInputButtonRectangle());
    TEST_ASSERT_EQUAL(LOW, animation->getInputButtonCircle());
    TEST_ASSERT_EQUAL(1000, animation->getCurrentTime());

    // Clean up
    std::cout << "Cleaning up..." << std::endl;
    delete animation;
}

void test_set_rotation_direction_random()
{
    std::cout << "  Running test_set_rotation_direction_random()" << std::endl;

    // Create mock objects
    const AnimationPins pins = AnimationPins();

    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });

    // Mock random number generator
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo(
            [](long min, long max)
            {
                return 100;  // Return a small number to favor left direction
            });

    // Create animation object
    Animation i(nullptr, nullptr, pins);

    // Set initial state
    i.setMotorDirection(MotorDirection::Right);
    i.setRandomRotateTimer(0);
    i.setLastLeftTurnTime(0);
    i.setLastRightTurnTime(0);
    i.setCurrentTime(1000);

    // Set sensor inputs to no trigger
    i.setInputSensorLeft(LOW);
    i.setInputSensorRight(LOW);

    // Test setRotationDirection
    std::cout << "Calling animation->setRotationDirection()..." << std::endl;
    i.setRotationDirection();

    // Verify direction was set to Right (left bias)
    std::cout << "Verifying direction was set to Right..." << std::endl;
    TEST_ASSERT_EQUAL(MotorDirection::Right, i.getMotorDirection());
    TEST_ASSERT_GREATER_THAN(0, i.getRandomRotateTimer());
}

void test_perform_rotate()
{
    std::cout << "  Running test_perform_rotate()" << std::endl;

    Mock<AudioPlayer> audioPlayerMock;

    // Create mock objects
    const AnimationPins pins = AnimationPins();
    Log.setLogLevel(LogLevel::NONE);

    // Mock random number generator to return a specific speed
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo(
            [](long min, long max)
            {
                return 200;  // Return a speed value
            });
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), millis)).AlwaysReturn(1000);
    When(Method(audioPlayerMock, play)).AlwaysReturn(true);

    // Create animation object
    Animation i(nullptr, &audioPlayerMock.get(), pins);

    // Set initial state
    i.setMotorDirection(MotorDirection::Right);
    i.setRandomRotateTimer(0);
    i.setLastLeftTurnTime(0);
    i.setLastRightTurnTime(0);
    i.setCurrentTime(1000);
    i.setLastPIRTimer(0);

    // Set sensor inputs to no trigger
    i.setInputPIRSensor(HIGH);
    i.setInputSensorLeft(LOW);
    i.setInputSensorRight(LOW);

    // Test performRotate
    std::cout << "Calling animation->performRotate()..." << std::endl;
    i.performRotate();

    TEST_ASSERT_GREATER_THAN(0, i.getLastPIRTimer());

    // Verify that last turn time was updated
    TEST_ASSERT_EQUAL(MotorDirection::Right, i.getMotorDirection());
}

void test_set_rotation_direction_with_sensor_trip()
{
    std::cout << "  Running test_set_rotation_direction_with_sensor_left()" << std::endl;
    // Create mock objects
    const AnimationPins pins = AnimationPins();

    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    // random not needed for this test
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    Animation i(nullptr, nullptr, pins);
    Log.setLogLevel(LogLevel::NONE);
    uint16_t currentTime = 1000;

    // Set initial state
    i.setMotorDirection(MotorDirection::Right);
    i.setRandomRotateTimer(0);
    i.setLastLeftTurnTime(0);
    i.setLastRightTurnTime(0);
    i.setCurrentTime(currentTime);
    i.setLastPIRTimer(0);

    // Set sensor inputs: PIR HIGH, SensorRight LOW
    i.setInputPIRSensor(HIGH);
    i.setInputSensorLeft(HIGH);
    i.setInputSensorRight(LOW);

    std::cout << "Calling animation->setRotationDirection() with SensorRight LOW..." << std::endl;
    i.setRotationDirection();

    // The direction should now be reversed (not Right)
    TEST_ASSERT_EQUAL(MotorDirection::Left, i.getMotorDirection());
    TEST_ASSERT_EQUAL(currentTime + AnimationConstants::kMinDirectionTime,
                      i.getRandomRotateTimer());

    // Set sensor inputs: PIR HIGH, SensorLeft LOW
    i.setInputPIRSensor(HIGH);
    i.setInputSensorLeft(LOW);
    i.setInputSensorRight(HIGH);

    std::cout << "Calling animation->setRotationDirection() with SensorLeft LOW..." << std::endl;
    i.setRotationDirection();

    // The direction should now be reversed (not Left)
    TEST_ASSERT_EQUAL(MotorDirection::Right, i.getMotorDirection());
    TEST_ASSERT_EQUAL(currentTime + AnimationConstants::kMinDirectionTime,
                      i.getRandomRotateTimer());
}

void test_perform_rotate_pir_not_triggered_no_timeout()
{
    std::cout << "  Running test_perform_rotate_pir_not_triggered_no_timeout()" << std::endl;
    // Create mock objects
    const AnimationPins pins = AnimationPins();

    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    Animation i(nullptr, nullptr, pins);
    Log.setLogLevel(LogLevel::NONE);

    // Set initial state
    i.setMotorDirection(MotorDirection::Right);
    i.setRandomRotateTimer(0);
    i.setLastLeftTurnTime(0);
    i.setLastRightTurnTime(0);
    i.setCurrentTime(1000);
    i.setLastPIRTimer(900);

    // Set sensor inputs: PIR LOW
    i.setInputPIRSensor(LOW);     // no movement
    i.setInputSensorLeft(HIGH);   // not triggered
    i.setInputSensorRight(HIGH);  // not triggered

    // Save state before
    auto beforePIRTimer = i.getLastPIRTimer();

    std::cout << "Calling animation->performRotate() with PIR LOW and no timeout..." << std::endl;
    i.performRotate();

    // Nothing should change
    TEST_ASSERT_EQUAL(beforePIRTimer, i.getLastPIRTimer());
    TEST_ASSERT_EQUAL(MotorDirection::Stop, i.getMotorDirection());
}

void test_perform_rotate_pir_not_triggered_timeout()
{
    std::cout << "  Running test_perform_rotate_pir_not_triggered_timeout()" << std::endl;
    Mock<AudioPlayer> audioPlayerMock;
    // Create mock objects
    const AnimationPins pins = AnimationPins();

    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();
    When(Method(audioPlayerMock, play)).AlwaysReturn(true);

    Animation i(nullptr, &audioPlayerMock.get(), pins);

    // Set initial state
    i.setMotorDirection(MotorDirection::Right);
    i.setRandomRotateTimer(0);
    i.setLastLeftTurnTime(0);
    i.setLastRightTurnTime(0);
    i.setCurrentTime(40000);
    i.setLastPIRTimer(0);

    // Set sensor inputs: PIR LOW
    i.setInputPIRSensor(LOW);
    i.setInputSensorLeft(LOW);
    i.setInputSensorRight(LOW);

    // Save state before
    auto beforePIRTimer = i.getLastPIRTimer();

    std::cout << "Calling animation->performRotate() with PIR LOW and timeout (should stop)..."
              << std::endl;
    i.performRotate();

    // m_lastPIRTimer should not change
    TEST_ASSERT_EQUAL(beforePIRTimer, i.getLastPIRTimer());
    // verify the actual invocation sequence contains two consecutive invocations of analogWrite
    // with b==LOW at least once.
    Verify(Method(ArduinoFake(), analogWrite).Matching([](int a, int b) { return b == LOW; }) * 2)
        .AtLeast(1);
}

void test_updateSound()
{
    std::cout << "  Running test_updateSound()" << std::endl;
    // Create mock objects
    Mock<AudioPlayer> audioPlayerMock;
    const AnimationPins pins = AnimationPins();
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    // playRandomSound and update are the key methods
    When(Method(audioPlayerMock, playRandomSound)).AlwaysReturn();
    When(Method(audioPlayerMock, update)).AlwaysReturn();

    Animation i(nullptr, &audioPlayerMock.get(), pins);

    // Case 1: Button LOW, not playing -> playRandomSound should be called
    i.setInputButtonRectangle(LOW);
    When(Method(audioPlayerMock, isPlaying)).AlwaysReturn(false);
    i.updateSound();
    Verify(Method(audioPlayerMock, playRandomSound)).Exactly(1);
    Verify(Method(audioPlayerMock, update)).Exactly(1);

    // Case 2: Button LOW, already playing -> playRandomSound should NOT be called
    audioPlayerMock.Reset();
    When(Method(audioPlayerMock, playRandomSound)).AlwaysReturn();
    When(Method(audioPlayerMock, update)).AlwaysReturn();
    When(Method(audioPlayerMock, isPlaying)).AlwaysReturn(true);
    i.setInputButtonRectangle(LOW);
    i.updateSound();
    Verify(Method(audioPlayerMock, playRandomSound)).Exactly(0);
    Verify(Method(audioPlayerMock, update)).Exactly(1);

    // Case 3: Button LOW -> playRandomSound should NOT be called
    audioPlayerMock.Reset();
    When(Method(audioPlayerMock, playRandomSound)).AlwaysReturn();
    When(Method(audioPlayerMock, update)).AlwaysReturn();
    When(Method(audioPlayerMock, isPlaying)).AlwaysReturn(false);

    // Case 4: Button HIGH -> playRandomSound should NOT be called
    i.setInputButtonRectangle(HIGH);
    i.updateSound();
    Verify(Method(audioPlayerMock, playRandomSound)).Exactly(0);
    Verify(Method(audioPlayerMock, update)).Exactly(1);
}

void test_rotate()
{
    std::cout << "  Running test_rotate()" << std::endl;

    // Setup mocks
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    // Create test pins
    AnimationPins pins;

    // Create animation object
    Animation animation(nullptr, nullptr, pins);
    Log.setLogLevel(LogLevel::NONE);

    // Test 1: Right direction
    {
        // Clear previous calls
        ArduinoFake().ClearInvocationHistory();

        // Call rotate with right direction
        animation.rotate(AnimationConstants::kMaxMotorSpeed, MotorDirection::Right);

        // Verify motor was set to right
        Verify(Method(ArduinoFake(), analogWrite)
                   .Using(pins.neckMotorIn2, AnimationConstants::kMaxMotorSpeed))
            .Once();
        Verify(Method(ArduinoFake(), analogWrite).Using(pins.neckMotorIn1, 0)).Once();
    }

    // Test 2: Left direction
    {
        // Reset mock call counters
        ArduinoFake().ClearInvocationHistory();

        // Call rotate with left direction
        animation.rotate(AnimationConstants::kMaxMotorSpeed, MotorDirection::Left);

        // Verify motor was set to left
        Verify(Method(ArduinoFake(), analogWrite)
                   .Using(pins.neckMotorIn1, AnimationConstants::kMaxMotorSpeed))
            .Once();
        Verify(Method(ArduinoFake(), analogWrite).Using(pins.neckMotorIn2, 0)).Once();
    }

    // Test 3: Stop
    {
        // Reset mock call counters
        ArduinoFake().ClearInvocationHistory();

        // Setup stop mock
        When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
            .Do([](const char* str) { return strlen(str); });

        // Call rotate with stop
        animation.rotate(AnimationConstants::kMaxMotorSpeed, MotorDirection::Stop);

        // Verify motor was stopped
        Verify(Method(ArduinoFake(), analogWrite).Using(pins.neckMotorIn1, 0)).Once();
        Verify(Method(ArduinoFake(), analogWrite).Using(pins.neckMotorIn2, 0)).Once();
    }
}

void test_perform_rotate_backward_duration()
{
    std::cout << "  Running test_perform_rotate_backward_duration()" << std::endl;

    Mock<AudioPlayer> audioPlayerMock;
    // Create mock objects
    const AnimationPins pins = AnimationPins();

    // Setup mocks
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    // When(Method(audioPlayerMock, play)).AlwaysReturn(true);
    // When(Method(audioPlayerMock, isPlaying)).AlwaysReturn(false);
    // When(Method(audioPlayerMock, stop)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo(
            [](long min, long max)
            {
                return min;  // Return minimum speed for consistent testing
            });
    When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
    When(Method(audioPlayerMock, play)).AlwaysReturn(true);

    // Create animation object
    Animation animation(nullptr, &audioPlayerMock.get(), pins);

    // Test case: Left direction
    const unsigned long baseTime = 1000;
    const unsigned long elapsedTime = 500;  // 500ms since last turn

    // Setup test conditions
    animation.setMotorDirection(MotorDirection::Left);
    animation.setLastRightTurnTime(baseTime);          // Set the last right turn time
    animation.setCurrentTime(baseTime + elapsedTime);  // Current time is 500ms later
    animation.setInputPIRSensor(HIGH);                 // Enable PIR sensor

    // Call the method
    animation.performRotate();

    // Verify the correct time was used for duration calculation
    // The test passes if it reaches this point without crashing
    // and the motor was set to move left
    Verify(Method(ArduinoFake(), analogWrite)
               .Matching([&](int pin, int speed) { return pin == pins.neckMotorIn2 && speed > 0; }))
        .AtLeastOnce();
}

void test_set_rotation_direction_bias()
{
    std::cout << "  Running test_set_rotation_direction_bias()" << std::endl;

    // Create mock objects
    const AnimationPins pins = AnimationPins();

    // Setup mocks
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    // Test cases for different bias scenarios
    struct TestCase
    {
        unsigned long timeSinceLeft;
        unsigned long timeSinceRight;
        float expectedLeftBias;
        float expectedRightBias;
    };

    std::vector<TestCase> testCases = {
        {100, 3000, 2.0f, 3.0f},  // Recently turned left, long time since right
        {3000, 100, 3.0f, 2.0f},  // Long time since left, recently turned right
        {100, 100, 2.0f, 2.0f},   // Recently turned both ways
        {3000, 3000, 3.0f, 3.0f}  // Long time since both turns
    };

    for (const auto& tc : testCases)
    {
        // Create animation object for each test case
        Animation animation(nullptr, nullptr, pins);

        // Set up test conditions
        const unsigned long baseTime = 10000;  // Arbitrary base time
        animation.setCurrentTime(baseTime);
        animation.setLastLeftTurnTime(baseTime - tc.timeSinceLeft);
        animation.setLastRightTurnTime(baseTime - tc.timeSinceRight);
        animation.setRandomRotateTimer(0);
        animation.setInputSensorLeft(HIGH);
        animation.setInputSensorRight(HIGH);

        // Debug output for test conditions
        std::cout << "  Test conditions - "
                  << "timeSinceLeft: " << tc.timeSinceLeft
                  << "ms, timeSinceRight: " << tc.timeSinceRight << "ms"
                  << ", leftBias: " << tc.expectedLeftBias
                  << ", rightBias: " << tc.expectedRightBias << std::endl;

        // Set random to always choose the first option (left)
        When(OverloadedMethod(ArduinoFake(), random, long(long))).AlwaysReturn(1000);

        // Call the method with debug output
        animation.setRotationDirection();

        // Debug output for actual direction and timer
        MotorDirection actualDirection = animation.getMotorDirection();
        std::cout << "  Result - Direction: "
                  << (actualDirection == MotorDirection::Right ? "Right" : "Left")
                  << ", Random Timer: " << static_cast<int>(animation.getRandomRotateTimer())
                  << " (current time: " << baseTime << ")" << std::endl;

        // Verify left bias was applied (Clockwise)
        TEST_ASSERT_EQUAL(MotorDirection::Right, actualDirection);

        // Set random to always choose the second option (right)
        When(OverloadedMethod(ArduinoFake(), random, long(long))).AlwaysReturn(0);

        animation.setRandomRotateTimer(0);
        // Call the method again
        animation.setRotationDirection();

        // Verify right bias was applied (CounterClockwise)
        TEST_ASSERT_EQUAL(MotorDirection::Left, animation.getMotorDirection());

        // Verify random timer was set
        TEST_ASSERT_GREATER_THAN(baseTime, animation.getRandomRotateTimer());
    }

    // Test timer expiration
    {
        Animation animation(nullptr, nullptr, pins);

        const unsigned long baseTime = 10000;
        animation.setCurrentTime(baseTime);
        animation.setRandomRotateTimer(baseTime - 1);  // Set timer to expire

        // Call the method
        animation.setRotationDirection();

        // Verify timer was reset
        TEST_ASSERT_EQUAL(baseTime + AnimationConstants::kMinDirectionTime,
                          animation.getRandomRotateTimer());
    }
}

void test_update()
{
    std::cout << "  Running test_update()" << std::endl;

    // Create mock objects
    Mock<EyeAnimation> pixelsMock;
    const AnimationPins pins = AnimationPins();
    When(Method(pixelsMock, setCurrentTime)).AlwaysReturn();

    // Create animation object
    Animation animation(&pixelsMock.get(), nullptr, pins);

    // Create test inputs
    AnimationInputs testInputs;
    testInputs.sensorLeft = HIGH;
    testInputs.sensorRight = LOW;
    testInputs.pirSensor = HIGH;
    testInputs.buttonRectangle = LOW;
    testInputs.buttonCircle = HIGH;
    testInputs.currentTime = 12345;

    // Call update with test inputs
    animation.update(testInputs);

    // Verify all inputs were updated correctly
    TEST_ASSERT_EQUAL(HIGH, animation.getInputSensorLeft());
    TEST_ASSERT_EQUAL(LOW, animation.getInputSensorRight());
    TEST_ASSERT_EQUAL(HIGH, animation.getInputPIRSensor());
    TEST_ASSERT_EQUAL(LOW, animation.getInputButtonRectangle());
    TEST_ASSERT_EQUAL(HIGH, animation.getInputButtonCircle());
    TEST_ASSERT_EQUAL(12345, animation.getCurrentTime());
}

void test_handle_pir_triggered_with_stopped_motor()
{
    std::cout << "  Running test_handle_pir_triggered_with_stopped_motor()" << std::endl;
    Mock<AudioPlayer> audioPlayerMock;
    // Create mock objects
    Animation obj(nullptr, &audioPlayerMock.get(), AnimationPins());
    Mock<Animation> spy(obj);
    When(OverloadedMethod(ArduinoFake(Stream), print, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str); });
    When(OverloadedMethod(ArduinoFake(Stream), println, size_t(const char*)))
        .AlwaysDo([](const char* str) { return strlen(str) + 1; });
    When(OverloadedMethod(ArduinoFake(), random, long(long, long)))
        .AlwaysDo(
            [](long min, long max)
            {
                return 100;  // Return a small number to favor left direction
            });
    When(Method(audioPlayerMock, play)).AlwaysReturn(true);

    // Set up test conditions
    Animation& animation = spy.get();

    // Set initial state - motor is stopped
    animation.setMotorDirection(MotorDirection::Stop);
    animation.setLastPIRState(LOW);  // Previous state was inactive
    animation.setCurrentTime(1000);  // Arbitrary time

    // Mock the setRotationDirection method
    When(Method(spy, setRotationDirection)).AlwaysReturn();

    // Mock the rotate method to verify it's called with expected parameters
    When(Method(spy, rotate)).AlwaysReturn();

    // Call the method under test
    animation.handlePirTriggered();

    // Verify state was updated
    TEST_ASSERT_EQUAL(HIGH, animation.getLastPIRState());
    TEST_ASSERT_EQUAL(1000, animation.getLastPIRTimer());
}

void runAnimationTests()
{
    std::cout << "\n==== Starting Animation Tests ====" << std::endl;
    RUN_TEST(test_animation_initialization);
    RUN_TEST(test_read_inputs);
    RUN_TEST(test_set_rotation_direction_with_sensor_trip);
    RUN_TEST(test_set_rotation_direction_random);
    RUN_TEST(test_set_rotation_direction_bias);
    RUN_TEST(test_update);
    RUN_TEST(test_perform_rotate);
    RUN_TEST(test_perform_rotate_backward_duration);
    RUN_TEST(test_perform_rotate_pir_not_triggered_no_timeout);
    RUN_TEST(test_perform_rotate_pir_not_triggered_timeout);
    RUN_TEST(test_rotate);
    RUN_TEST(test_handle_pir_triggered_with_stopped_motor);
    RUN_TEST(test_updateSound);
}
