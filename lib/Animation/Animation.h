/**
 * @file Animation.h
 * @brief Animation controller for Y-Series USB Hub
 */

#ifndef Y_SERIES_USB_HUB_ANIMATION_H
#define Y_SERIES_USB_HUB_ANIMATION_H

#include <cstdint>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Logger.h>
#include <AudioPlayer.h>
#include "AnimationPins.h"
#include "AnimationInputs.h"

/**
 * @brief Animation timing and control constants
 */
namespace AnimationConstants {
    // Motor control
    constexpr uint8_t kMaxMotorSpeed = 255;           ///< Maximum allowed motor speed (0-255)
    constexpr uint8_t kMinSpeed = 80;                 ///< Minimum motor speed for operation
    constexpr uint32_t kSpeedRampTime = 2000;         ///< Time (ms) to ramp speed up/down
    
    // Rotation timing
    constexpr uint32_t kMinRotateInterval = 500;      ///< Minimum time (ms) between direction changes
    constexpr uint32_t kMaxRotateInterval = 1000;     ///< Maximum time (ms) between direction changes
    constexpr uint32_t kMinDirectionTime = 200;       ///< Min time (ms) before considering direction change
    constexpr uint32_t kMaxDirectionTime = 2000;      ///< Time (ms) after which direction is strongly preferred
    
    // PIR sensor timing
    constexpr uint32_t kPirTimeout = 30000;           ///< Time (ms) after PIR trigger before stopping
    constexpr uint32_t kInactivityTimeout = 30000;    ///< Time (ms) of inactivity before stopping
    
    // Direction bias
    constexpr float kNormalBias = 1.0f;               ///< Base direction bias
    constexpr float kStrongBias = 2.0f;               ///< Bias when recently turned in a direction
    constexpr float kStrongerBias = 3.0f;             ///< Bias when it's been a long time since turning
       
} // namespace AnimationConstants


/**
 * @brief Direction of motor rotation
 */
enum class MotorDirection : int8_t {
    Stop = 0,     ///< Motor is stopped
    Forward = 1,  ///< Motor is moving forward
    Backward = -1 ///< Motor is moving backward
};

/**
 * @brief Compound assignment operator for MotorDirection and int
 * @param[in,out] lhs MotorDirection to modify
 * @param[in] rhs Multiplier
 * @return Reference to the modified MotorDirection
 */
inline MotorDirection& operator*=(MotorDirection& lhs, int rhs) {
    return lhs = static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}

/**
 * @brief Multiplication operator for int and MotorDirection
 * @param[in] lhs Multiplier
 * @param[in] rhs MotorDirection to multiply
 * @return Resulting MotorDirection
 */
inline MotorDirection operator*(int lhs, MotorDirection rhs) {
    return static_cast<MotorDirection>(lhs * static_cast<int8_t>(rhs));
}

/**
 * @brief Multiplication operator for MotorDirection and int
 * @param[in] lhs MotorDirection to multiply
 * @param[in] rhs Multiplier
 * @return Resulting MotorDirection
 */
inline MotorDirection operator*(MotorDirection lhs, int rhs) {
    return static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}


/**
 * @brief Main animation controller class
 */
class Animation {
public:
    /**
     * @brief Construct a new Animation object
     * @param[in] serial Serial port for logging
     * @param[in] pixels NeoPixel controller
     * @param[in] audio Audio player instance
     * @param[in] pins Pin configuration
     */
    Animation(Stream* serial,
              Adafruit_NeoPixel* pixels, 
              AudioPlayer* audio, 
              const AnimationPins& pins)
        : m_pixels(pixels)
        , m_audioPlayer(audio)
        , m_logger(serial, "[Animation] ")
        , m_pins(pins) {
    }

    virtual ~Animation() = default;

    void update(const AnimationInputs& inputs);
    virtual void setRotationDirection();
    virtual void performRotate();

    // Getters for testing
    int8_t getInputSensorLeft() const { return m_inputSensorLeft; }
    int8_t getInputSensorRight() const { return m_inputSensorRight; }
    int8_t getInputPIRSensor() const { return m_inputPIRSensor; }
    int8_t getInputButtonRectangle() const { return m_inputButtonRectangle; }
    int8_t getInputButtonCircle() const { return m_inputButtonCircle; }
    unsigned long getCurrentTime() const { return m_currentTime; }
    MotorDirection getMotorDirection() const { return m_motorDirection; }
    unsigned long getRandomRotateTimer() const { return m_randomRotateTimer; }
    unsigned long getLastLeftTurnTime() const { return m_lastLeftTurnTime; }
    unsigned long getLastRightTurnTime() const { return m_lastRightTurnTime; }
    unsigned long getLastPIRTimer() const { return m_lastPIRTimer; }
    bool getIsRainbowActive() const { return m_isRainbowActive; }
    uint8_t getRainbowIndex() const { return m_rainbowIndex; }
    unsigned long getRainbowTimer() const { return m_rainbowTimer; }

    // Setters for testing
    void setInputSensorLeft(int8_t value) { m_inputSensorLeft = value; }
    void setInputSensorRight(int8_t value) { m_inputSensorRight = value; }
    void setInputPIRSensor(int8_t value) { m_inputPIRSensor = value; }
    void setInputButtonRectangle(int8_t value) { m_inputButtonRectangle = value; }
    void setInputButtonCircle(int8_t value) { m_inputButtonCircle = value; }
    void setCurrentTime(unsigned long value) { m_currentTime = value; }
    void setMotorDirection(MotorDirection value) { m_motorDirection = value; }
    void setRandomRotateTimer(unsigned long value) { m_randomRotateTimer = value; }
    void setLastLeftTurnTime(unsigned long value) { m_lastLeftTurnTime = value; }
    void setLastRightTurnTime(unsigned long value) { m_lastRightTurnTime = value; }
    void setLastPIRTimer(unsigned long value) { m_lastPIRTimer = value; }
    void setIsRainbowActive(bool value) { m_isRainbowActive = value; }
    void setRainbowIndex(uint8_t value) { m_rainbowIndex = value; }
    void setRainbowTimer(unsigned long value) { m_rainbowTimer = value; }

    virtual void eyeBlink();
    virtual void updateSound();
    virtual void rotate(uint8_t speed, MotorDirection direction);
    virtual void stop();
    virtual uint32_t wheel(uint8_t pos);
    virtual void updateRainbow();

    // Helper methods
    void handlePirTriggered();
    void handlePirInactive();

protected:
    AnimationPins m_pins;
    Adafruit_NeoPixel* m_pixels = nullptr;
    Logger m_logger;
    AudioPlayer* m_audioPlayer = nullptr;
    MotorDirection m_motorDirection = MotorDirection::Stop;
    int8_t m_inputSensorLeft;
    int8_t m_inputSensorRight;
    int8_t m_inputPIRSensor;
    int8_t m_lastPIRState;  // Track previous PIR state for edge detection
    int8_t m_inputButtonRectangle;
    int8_t m_inputButtonCircle;
    unsigned long m_lastPIRTimer;
    unsigned long m_randomRotateTimer;
    unsigned long m_lastLeftTurnTime;
    unsigned long m_lastRightTurnTime;

    uint8_t m_rainbowIndex;  // Current position in rainbow cycle
    unsigned long m_rainbowTimer; // Timer for rainbow animation
    bool m_isRainbowActive;   // Flag to track rainbow state
    unsigned long m_currentTime;      // Timestamp from update() call

};

#endif // Y_SERIES_USB_HUB_ANIMATION_H