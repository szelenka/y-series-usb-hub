/**
 * @file Animation.h
 * @brief Main animation controller for the Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file defines the Animation class which manages all animation-related functionality
 * including motor control, LED effects, and sensor input handling. It coordinates the
 * behavior of the device based on various inputs and timing conditions.
 *
 * The Animation class is responsible for:
 * - Managing motor control for head movement
 * - Coordinating LED animations and effects
 * - Processing sensor inputs (PIR, limit switches, buttons)
 * - Handling state transitions and timing
 * - Managing sound effect playback
 */

#ifndef Y_SERIES_USB_HUB_ANIMATION_H
#define Y_SERIES_USB_HUB_ANIMATION_H

// System includes
#include <Arduino.h>
#include <algorithm>
#include <cstdint>

// Project includes
#include "AnimationInputs.h"
#include "AnimationPins.h"
#include <EyeAnimation.h>
#include <AudioPlayer.h>
#include <Logger.h>

/**
 * @brief Namespace containing all animation-related constants
 *
 * This namespace groups all timing, control, and configuration constants
 * used throughout the animation system. These values control the behavior
 * of motors, sensors, and visual effects.
 */
namespace AnimationConstants
{
/// @name Motor Control
/// @{
constexpr uint8_t kMaxMotorSpeed = 112;           ///< Maximum allowed motor speed (0-255)
constexpr uint8_t kMinSpeed = 80;                 ///< Minimum motor speed for operation
constexpr uint32_t kSpeedRampTime = 500;          //< Time (ms) to ramp speed up/down
constexpr uint32_t kMinMovementInterval = 5000;   // Min time between movements (ms)
constexpr uint32_t kMaxMovementInterval = 25000;  // Max time between movements (ms)
constexpr uint32_t kMinMovementDuration = 500;    // Min duration of a single movement (ms)
constexpr uint32_t kMaxMovementDuration = 2000;   // Max duration of a single movement (ms)
constexpr uint8_t kMovementChance = 30;           // 30% chance to move when motion is detected

/// @}

/// @name Rotation Timing
/// @{
constexpr uint32_t kMinRotateInterval = 500;   ///< Minimum time (ms) between direction changes
constexpr uint32_t kMaxRotateInterval = 1000;  ///< Maximum time (ms) between direction changes
constexpr uint32_t kMinDirectionTime = 500;  ///< Min time (ms) before considering direction change
constexpr uint32_t kMaxDirectionTime =
    1500;  ///< Time (ms) after which direction is strongly preferred
/// @}

/// @name PIR Sensor Timing
/// @{
constexpr uint32_t kInactivityTimeout = 5000;  ///< Time (ms) of inactivity before stopping
constexpr uint32_t kEyeResetInterval =
    300000;  ///< Time (ms) before eye animation reset (5 minutes)
/// @}

/// @name Direction Bias
/// @{
constexpr float kNormalBias = 1.0f;    ///< Base direction bias
constexpr float kStrongBias = 2.0f;    ///< Bias when recently turned in a direction
constexpr float kStrongerBias = 3.0f;  ///< Bias when it's been a long time since turning
/// @}

/// @name LED Fade
/// @{
constexpr uint8_t kLedFadeIncrement = 5;    ///< How much to change brightness each step
constexpr uint32_t kLedFadeInterval = 30;   ///< Time (ms) between fade steps
constexpr uint8_t kLedMinBrightness = 64;   ///< Minimum LED brightness (0-255)
constexpr uint8_t kLedMaxBrightness = 128;  ///< Maximum LED brightness (0-255)
/// @}
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
    Stop = 0,   ///< Motor is not moving
    Right = 1,  ///< Motor is rotating to the right
    Left = -1   ///< Motor is rotating to the left
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
 * @details
 * The Animation class manages the core functionality of the device, including:
 * - Motor control for head movement with smooth acceleration/deceleration
 * - LED animations and visual feedback
 * - Sensor input processing (PIR, limit switches, buttons)
 * - Sound effect coordination through AudioPlayer
 * - State management for interactive behaviors
 *
 * The class implements a state machine that responds to sensor inputs and
 * timing conditions to create natural, engaging animations and interactions.
 */
class Animation
{
public:
    /**
     * @brief Construct a new Animation controller
     *
     * @param[in] eye Pointer to the EyeAnimation controller for LED operations
     * @param[in] audio Pointer to the AudioPlayer instance for sound effects
     * @param[in] pins Pin configuration structure with all hardware pin assignments
     *
     * @note The constructor initializes all hardware components to a known state
     *       and sets up the initial animation state.
     */
    Animation(EyeAnimation* eye, AudioPlayer* audio, const AnimationPins& pins);

    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~Animation() = default;

    // Prevent copying and assignment
    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;

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
     * @brief Controls motor rotation with specified speed and direction
     *
     * @param[in] speed Motor speed (0-255)
     * @param[in] direction Direction of rotation (Right/Left/Stop)
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
     * @brief Updates sound effects based on current state
     *
     * Manages audio playback for various animations and interactions.
     * Handles sound effect triggering, volume control, and state transitions.
     */
    virtual void updateSound();

    /**
     * @brief Controls the eye blinking animation
     *
     * Manages the timing and visual effect of the eye blinking animation.
     * This method should be called regularly to update the animation state.
     */
    virtual void eyeBlink();

    /// @name Getters
    /// @{
    /**
     * @brief Get the current motor direction
     * @return Current MotorDirection value
     */
    MotorDirection getMotorDirection() const { return m_motorDirection; }

    /**
     * @brief Get the current timestamp from the last update
     * @return Current system time in milliseconds
     */
    unsigned long getCurrentTime() const { return m_currentTime; }
    /// @}

    /// @name Testing Interface
    /// @{
    // The following methods are primarily for testing purposes
    int8_t getInputSensorLeft() const { return m_inputSensorLeft; }
    int8_t getInputSensorRight() const { return m_inputSensorRight; }
    int8_t getInputPIRSensor() const { return m_inputPIRSensor; }
    int8_t getInputButtonRectangle() const { return m_inputButtonRectangle; }
    int8_t getInputButtonCircle() const { return m_inputButtonCircle; }
    unsigned long getRandomRotateTimer() const { return m_randomRotateTimer; }
    unsigned long getRandomDirectionTimer() const { return m_randomDirectionTimer; }
    unsigned long getLastLeftTurnTime() const { return m_lastLeftTurnTime; }
    unsigned long getLastRightTurnTime() const { return m_lastRightTurnTime; }
    unsigned long getLastPIRTimer() const { return m_lastPIRTimer; }
    int8_t getLastPIRState() const { return m_lastPIRState; }

    void setInputSensorLeft(int8_t value) { m_inputSensorLeft = value; }
    void setInputSensorRight(int8_t value) { m_inputSensorRight = value; }
    void setInputPIRSensor(int8_t value) { m_inputPIRSensor = value; }
    void setInputButtonRectangle(int8_t value) { m_inputButtonRectangle = value; }
    void setInputButtonCircle(int8_t value) { m_inputButtonCircle = value; }
    void setCurrentTime(unsigned long value) { m_currentTime = value; }
    void setMotorDirection(MotorDirection value) { m_motorDirection = value; }
    void setRandomRotateTimer(unsigned long value) { m_randomRotateTimer = value; }
    void setRandomDirectionTimer(unsigned long value) { m_randomDirectionTimer = value; }
    void setLastLeftTurnTime(unsigned long value) { m_lastLeftTurnTime = value; }
    void setLastRightTurnTime(unsigned long value) { m_lastRightTurnTime = value; }
    void setLastPIRTimer(unsigned long value) { m_lastPIRTimer = value; }
    void setLastPIRState(int8_t value) { m_lastPIRState = value; }
    /// @}

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

    /**
     * @brief Updates LED fade animation
     *
     * Manages the LED fade animation for the dome LED.
     */
    void updateLedFade();

protected:
    /// @name Hardware Interfaces
    /// @{
    AnimationPins m_pins;                    ///< Pin configuration for all hardware components
    EyeAnimation* m_eyeAnimation = nullptr;  ///< Controller for NeoPixel LEDs
    AudioPlayer* m_audioPlayer = nullptr;    ///< Audio playback controller
    /// @}

    /// @name Motor Control State
    /// @{
    MotorDirection m_motorDirection =
        MotorDirection::Stop;                  ///< Current direction of motor movement
    unsigned long m_lastLeftTurnTime = 0;      ///< Timestamp of last left turn
    unsigned long m_lastRightTurnTime = 0;     ///< Timestamp of last right turn
    unsigned long m_randomRotateTimer = 0;     ///< Timer for random rotation timing
    unsigned long m_randomDirectionTimer = 0;  ///< Timer for random direction timing
    uint32_t m_lastMovementEndTime = 0;        ///< When the last movement ended
    bool m_isInMovementCycle = false;          ///< Whether we're currently in a movement cycle
    /// @}

    /// @name LED Fade State
    /// @{
    uint8_t m_currentLedBrightness = 0;  ///< Current LED brightness (0-255)
    bool m_ledFadeDirection = true;      ///< true = fading up, false = fading down
    unsigned long m_lastFadeTime = 0;    ///< Last time the LED was faded
    /// @}

    /// @name Sensor States
    /// @{
    int8_t m_inputSensorLeft = 0;       ///< Current state of left hall effect sensor
    int8_t m_inputSensorRight = 0;      ///< Current state of right hall effect sensor
    int8_t m_inputPIRSensor = 0;        ///< Current state of PIR motion sensor
    int8_t m_lastPIRState = 0;          ///< Previous state of PIR sensor (for edge detection)
    int8_t m_inputButtonRectangle = 0;  ///< Current state of rectangular button
    int8_t m_inputButtonCircle = 0;     ///< Current state of circular button
    unsigned long m_lastPIRTimer = 0;   ///< Timestamp of last PIR sensor trigger
    /// @}

    /// @name System State
    /// @{
    unsigned long m_currentTime = 0;  ///< Current system time from last update() call
    /// @}
};

#endif  // Y_SERIES_USB_HUB_ANIMATION_H