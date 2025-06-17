#include <ArduinoFake.h>
#include <unity.h>

#include "../lib/EyeAnimation/EyeAnimation.h"

using namespace fakeit;

void test_eye_animation_initialization()
{
    std::cout << "  Running test_eye_animation_initialization()" << std::endl;

    // Create mock NeoPixel
    Mock<Adafruit_NeoPixel> pixelsMock;
    When(Method(pixelsMock, numPixels)).AlwaysReturn(16);
    When(Method(pixelsMock, setPixelColor)).AlwaysReturn();
    When(Method(pixelsMock, show)).AlwaysReturn();

    // Create eye animation
    EyeAnimation eye(&pixelsMock.get());

    // Verify initialization
    Verify(Method(pixelsMock, setPixelColor).Using(Any(), 0));
}

void test_eye_animation_update_rainbow()
{
    std::cout << "  Running test_eye_animation_update_rainbow()" << std::endl;

    // Create mock NeoPixel
    Mock<Adafruit_NeoPixel> pixelsMock;
    When(Method(pixelsMock, numPixels)).AlwaysReturn(16);
    When(Method(pixelsMock, setPixelColor)).AlwaysReturn();
    When(Method(pixelsMock, show)).AlwaysReturn();

    // Create eye animation
    EyeAnimation eye(&pixelsMock.get());

    // Test rainbow update
    eye.setCurrentTime(0);  // Button pressed

    // Verify pixels were updated
    Verify(Method(pixelsMock, setPixelColor)).AtLeastOnce();
}

void test_eye_animation_set_color()
{
    std::cout << "  Running test_eye_animation_set_color()" << std::endl;

    // Create mock NeoPixel
    Mock<Adafruit_NeoPixel> pixelsMock;
    When(Method(pixelsMock, numPixels)).AlwaysReturn(16);
    When(Method(pixelsMock, setPixelColor)).AlwaysReturn();
    When(Method(pixelsMock, show)).AlwaysReturn();

    // Create eye animation
    EyeAnimation eye(&pixelsMock.get());

    // Set color and update
    eye.setActiveColor(0x123456);
    eye.setCurrentTime(0);
}

void test_eye_animation_set_brightness()
{
    std::cout << "  Running test_eye_animation_set_brightness()" << std::endl;

    // Create mock NeoPixel
    Mock<Adafruit_NeoPixel> pixelsMock;
    When(Method(pixelsMock, numPixels)).AlwaysReturn(16);
    When(Method(pixelsMock, setPixelColor)).AlwaysReturn();
    When(Method(pixelsMock, show)).AlwaysReturn();

    // Create eye animation
    EyeAnimation eye(&pixelsMock.get());

    // Set brightness and update
    eye.setBrightness(128);
    eye.setCurrentTime(0);

    // Verify brightness was applied
    // Note: Exact verification would require checking the scaled color values
    Verify(Method(pixelsMock, setPixelColor)).AtLeastOnce();
}

void runEyeAnimationTests()
{
    std::cout << "\n==== Starting Eye Animation Tests ====" << std::endl;
    RUN_TEST(test_eye_animation_initialization);
    RUN_TEST(test_eye_animation_update_rainbow);
    RUN_TEST(test_eye_animation_set_color);
    RUN_TEST(test_eye_animation_set_brightness);
}
