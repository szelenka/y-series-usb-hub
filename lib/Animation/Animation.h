/**
 * @file Animation.h
 * @brief Main animation controller for the Y-Series USB Hub
 *
 * This file defines the Animation class which manages all animation-related functionality
 * including motor control, LED effects, and sensor input handling. It coordinates the
 * behavior of the device based on various inputs and timing conditions.
 */

#ifndef Y_SERIES_USB_HUB_ANIMATION_H
#define Y_SERIES_USB_HUB_ANIMATION_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <AudioPlayer.h>
#include <Logger.h>

#include <algorithm>
#include <cstdint>

#include "AnimationInputs.h"
#include "AnimationPins.h"

/**
 * @brief Namespace containing all animation-related constants
 *
 * This namespace groups all timing, control, and configuration constants
 * used throughout the animation system. These values control the behavior
 * of motors, sensors, and visual effects.
 */
namespace AnimationConstants
{
// Motor control
constexpr uint8_t kMaxMotorSpeed = 255;    ///< Maximum allowed motor speed (0-255)
constexpr uint8_t kMinSpeed = 80;          ///< Minimum motor speed for operation
constexpr uint32_t kSpeedRampTime = 2000;  ///< Time (ms) to ramp speed up/down

// Rotation timing
constexpr uint32_t kMinRotateInterval = 500;   ///< Minimum time (ms) between direction changes
constexpr uint32_t kMaxRotateInterval = 1000;  ///< Maximum time (ms) between direction changes
constexpr uint32_t kMinDirectionTime = 200;  ///< Min time (ms) before considering direction change
constexpr uint32_t kMaxDirectionTime =
    2000;  ///< Time (ms) after which direction is strongly preferred

// PIR sensor timing
constexpr uint32_t kPirTimeout = 30000;         ///< Time (ms) after PIR trigger before stopping
constexpr uint32_t kInactivityTimeout = 30000;  ///< Time (ms) of inactivity before stopping

// Direction bias
constexpr float kNormalBias = 1.0f;    ///< Base direction bias
constexpr float kStrongBias = 2.0f;    ///< Bias when recently turned in a direction
constexpr float kStrongerBias = 3.0f;  ///< Bias when it's been a long time since turning

}  // namespace AnimationConstants

/**
 * @brief Enumerates possible directions of motor rotation
 *
 * This enum class represents the direction in which a motor can rotate,
 * including the stopped state. It's implemented as an 8-bit signed integer
 * to support multiplication with direction factors.
 */
enum class MotorDirection : int8_t
{
    Stop = 0,      ///< Motor is not moving
    Forward = 1,   ///< Motor is rotating in the forward direction
    Backward = -1  ///< Motor is rotating in the backward direction
};

/**
 * @brief Compound assignment operator for MotorDirection and int
 *
 * Multiplies a MotorDirection by an integer scalar in place. This allows
 * for easy direction reversal or stopping by multiplying by -1 or 0.
 *
 * @param[in,out] lhs MotorDirection to modify
 * @param[in] rhs Integer multiplier (typically -1, 0, or 1)
 * @return Reference to the modified MotorDirection
 */
inline MotorDirection& operator*=(MotorDirection& lhs, int rhs)
{
    return lhs = static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}

/**
 * @brief Multiplication operator for int and MotorDirection
 *
 * Allows multiplying a direction by a scalar from the left.
 *
 * @param[in] lhs Integer multiplier
 * @param[in] rhs MotorDirection to multiply
 * @return Resulting MotorDirection after multiplication
 *
 * @note This is a commutative operation with operator*(MotorDirection, int)
 */
inline MotorDirection operator*(int lhs, MotorDirection rhs)
{
    return static_cast<MotorDirection>(lhs * static_cast<int8_t>(rhs));
}

/**
 * @brief Multiplication operator for MotorDirection and int
 *
 * Allows multiplying a direction by a scalar from the right.
 *
 * @param[in] lhs MotorDirection to multiply
 * @param[in] rhs Integer multiplier
 * @return Resulting MotorDirection after multiplication
 */
inline MotorDirection operator*(MotorDirection lhs, int rhs)
{
    return static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}

/**
 * @brief Main controller for all animation and interaction behaviors
 *
 * The Animation class manages the core functionality of the device, including:
 * - Motor control for head movement
 * - LED animations and effects
 * - Sensor input processing
 * - Sound effect coordination
 * - State management for interactive behaviors
 *
 * This class serves as the central coordinator between hardware components
 * and implements the main behavior logic of the device.
 */
class Animation
{
public:
    /**
     * @brief Construct a new Animation controller
     *
     * Initializes the animation system with the specified hardware interfaces.
     * The constructor sets up the logger and stores references to all required
     * hardware components.
     *
     * @param[in] serial Pointer to the Stream object used for debug logging
     * @param[in] pixels Pointer to the NeoPixel controller for LED operations
     * @param[in] audio Pointer to the AudioPlayer instance for sound effects
     * @param[in] pins Pin configuration structure with all hardware pin assignments
     *
     * @note The caller is responsible for ensuring all pointer parameters remain
     *       valid for the lifetime of the Animation object.
     */
    Animation(Stream* serial, Adafruit_NeoPixel* pixels, AudioPlayer* audio,
              const AnimationPins& pins)
        : m_pixels(pixels), m_audioPlayer(audio), m_logger(serial, "[Animation] "), m_pins(pins)
    {
        // Initialize member variables to default states
        m_motorDirection = MotorDirection::Stop;
        m_rainbowIndex = 0;
        m_rainbowTimer = 0;
        m_isRainbowActive = false;
        m_currentTime = 0;
        m_lastPIRState = LOW;
        m_lastPIRTimer = 0;
        m_randomRotateTimer = 0;
        m_lastLeftTurnTime = 0;
        m_lastRightTurnTime = 0;
    }

    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~Animation() = default;

    /**
     * @brief Main update function called in the main loop
     *
     * Processes all inputs, updates the animation state, and controls outputs.
     * This method should be called regularly from the main program loop.
     *
     * @param[in] inputs Current state of all input devices
     */
    void update(const AnimationInputs& inputs);

    /**
     * @brief Determines the direction for the next rotation
     *
     * This method uses sensor inputs and timing to decide the next rotation
     * direction, implementing the core behavior logic for head movement.
     * It considers factors like recent movements, sensor triggers, and timing.
     */
    virtual void setRotationDirection();

    /**
     * @brief Executes the rotation behavior based on current state
     *
     * Manages the motor control for head rotation, including speed ramping,
     * timing, and interaction with the PIR sensor. This method handles the
     * actual movement logic once the direction has been determined.
     */
    virtual void performRotate();

    // Getters for testing
    /// @name Getters for Testing
    /// @{

    /// @brief Get the state of the left hall effect sensor
    int8_t getInputSensorLeft() const { return m_inputSensorLeft; }

    /// @brief Get the state of the right hall effect sensor
    int8_t getInputSensorRight() const { return m_inputSensorRight; }

    /// @brief Get the state of the PIR motion sensor
    int8_t getInputPIRSensor() const { return m_inputPIRSensor; }

    /// @brief Get the state of the rectangular button
    int8_t getInputButtonRectangle() const { return m_inputButtonRectangle; }

    /// @brief Get the state of the circular button
    int8_t getInputButtonCircle() const { return m_inputButtonCircle; }

    /// @brief Get the current timestamp from the last update
    unsigned long getCurrentTime() const { return m_currentTime; }

    /// @brief Get the current motor direction
    MotorDirection getMotorDirection() const { return m_motorDirection; }

    /// @brief Get the timer used for random rotation timing
    unsigned long getRandomRotateTimer() const { return m_randomRotateTimer; }

    /// @brief Get the timestamp of the last left turn
    unsigned long getLastLeftTurnTime() const { return m_lastLeftTurnTime; }

    /// @brief Get the timestamp of the last right turn
    unsigned long getLastRightTurnTime() const { return m_lastRightTurnTime; }

    /// @brief Get the timestamp of the last PIR sensor trigger
    unsigned long getLastPIRTimer() const { return m_lastPIRTimer; }

    /// @brief Check if rainbow animation is currently active
    bool getIsRainbowActive() const { return m_isRainbowActive; }

    /// @brief Get the current position in the rainbow color cycle
    uint8_t getRainbowIndex() const { return m_rainbowIndex; }

    /// @brief Get the timer used for rainbow animation timing
    unsigned long getRainbowTimer() const { return m_rainbowTimer; }

    /// @brief Get the previous state of the PIR sensor
    int8_t getLastPIRState() const { return m_lastPIRState; }

    /// @}

    // Setters for testing
    /// @name Setters for Testing
    /// @{

    /// @brief Set the state of the left hall effect sensor
    void setInputSensorLeft(int8_t value) { m_inputSensorLeft = value; }

    /// @brief Set the state of the right hall effect sensor
    void setInputSensorRight(int8_t value) { m_inputSensorRight = value; }

    /// @brief Set the state of the PIR motion sensor
    void setInputPIRSensor(int8_t value) { m_inputPIRSensor = value; }

    /// @brief Set the state of the rectangular button
    void setInputButtonRectangle(int8_t value) { m_inputButtonRectangle = value; }

    /// @brief Set the state of the circular button
    void setInputButtonCircle(int8_t value) { m_inputButtonCircle = value; }

    /// @brief Set the current timestamp (for testing time-based behavior)
    void setCurrentTime(unsigned long value) { m_currentTime = value; }

    /// @brief Set the motor direction
    void setMotorDirection(MotorDirection value) { m_motorDirection = value; }

    /// @brief Set the random rotation timer
    void setRandomRotateTimer(unsigned long value) { m_randomRotateTimer = value; }

    /// @brief Set the timestamp of the last left turn
    void setLastLeftTurnTime(unsigned long value) { m_lastLeftTurnTime = value; }

    /// @brief Set the timestamp of the last right turn
    void setLastRightTurnTime(unsigned long value) { m_lastRightTurnTime = value; }

    /// @brief Set the timestamp of the last PIR sensor trigger
    void setLastPIRTimer(unsigned long value) { m_lastPIRTimer = value; }

    /// @brief Set whether rainbow animation is active
    void setIsRainbowActive(bool value) { m_isRainbowActive = value; }

    /// @brief Set the current position in the rainbow color cycle
    void setRainbowIndex(uint8_t value) { m_rainbowIndex = value; }

    /// @brief Set the rainbow animation timer
    void setRainbowTimer(unsigned long value) { m_rainbowTimer = value; }

    /// @brief Set the previous state of the PIR sensor
    void setLastPIRState(int8_t value) { m_lastPIRState = value; }

    /// @}

    /**
     * @brief Controls the eye blinking animation
     *
     * Manages the timing and visual effect of the eye blinking animation.
     * This method should be called regularly to update the animation state.
     */
    virtual void eyeBlink();

    /**
     * @brief Updates sound effects based on current state
     *
     * Manages audio playback for various animations and interactions.
     * Handles sound effect triggering, volume control, and state transitions.
     */
    virtual void updateSound();

    /**
     * @brief Controls motor rotation with specified speed and direction
     *
     * @param[in] speed Motor speed (0-255)
     * @param[in] direction Direction of rotation (Forward/Backward/Stop)
     *
     * @note Actual speed may be limited by kMinSpeed and kMaxMotorSpeed constants
     */
    virtual void rotate(uint8_t speed, MotorDirection direction);

    /**
     * @brief Stops all motor movement
     *
     * Immediately stops the motor and cleans up any related state.
     * This is a hard stop with no ramping.
     */
    virtual void stop();

    /**
     * @brief Generates a color value for rainbow effects
     *
     * @param[in] pos Position in the color wheel (0-255)
     * @return uint32_t Color value in 0x00RRGGBB format
     *
     * This implements a color wheel that smoothly transitions between
     * red, green, and blue values to create rainbow effects.
     */
    virtual uint32_t wheel(uint8_t pos);

    /**
     * @brief Updates the rainbow LED animation
     *
     * Manages the timing and color progression of the rainbow effect.
     * Should be called regularly for smooth animation.
     */
    virtual void updateRainbow();

    // Helper methods
    /**
     * @brief Handles actions when PIR sensor is triggered
     *
     * Contains logic to execute when motion is detected by the PIR sensor.
     * This includes starting animations and setting appropriate timers.
     */
    void handlePirTriggered();

    /**
     * @brief Handles actions when PIR sensor becomes inactive
     *
     * Contains logic to execute when motion is no longer detected.
     * Manages the transition to idle state and related animations.
     */
    void handlePirInactive();

protected:
    /// @name Hardware Interfaces
    /// @{
    AnimationPins m_pins;                   ///< Pin configuration for all hardware components
    Adafruit_NeoPixel* m_pixels = nullptr;  ///< Controller for NeoPixel LEDs
    AudioPlayer* m_audioPlayer = nullptr;   ///< Audio playback controller
    /// @}

    /// @name Motor Control State
    /// @{
    MotorDirection m_motorDirection =
        MotorDirection::Stop;           ///< Current direction of motor movement
    unsigned long m_lastLeftTurnTime;   ///< Timestamp of last left turn
    unsigned long m_lastRightTurnTime;  ///< Timestamp of last right turn
    unsigned long m_randomRotateTimer;  ///< Timer for random rotation timing
    /// @}

    /// @name Sensor States
    /// @{
    int8_t m_inputSensorLeft;       ///< Current state of left hall effect sensor
    int8_t m_inputSensorRight;      ///< Current state of right hall effect sensor
    int8_t m_inputPIRSensor;        ///< Current state of PIR motion sensor
    int8_t m_lastPIRState;          ///< Previous state of PIR sensor (for edge detection)
    int8_t m_inputButtonRectangle;  ///< Current state of rectangular button
    int8_t m_inputButtonCircle;     ///< Current state of circular button
    unsigned long m_lastPIRTimer;   ///< Timestamp of last PIR sensor trigger
    /// @}

    /// @name LED Animation State
    /// @{
    uint8_t m_rainbowIndex;        ///< Current position in rainbow color cycle (0-255)
    unsigned long m_rainbowTimer;  ///< Timer for rainbow animation timing
    bool m_isRainbowActive;        ///< Flag indicating if rainbow animation is active
    /// @}

    /// @name System State
    /// @{
    Logger m_logger;              ///< Logging interface for debug output
    unsigned long m_currentTime;  ///< Current system time from last update() call
    /// @}
};

#endif  // Y_SERIES_USB_HUB_ANIMATION_H