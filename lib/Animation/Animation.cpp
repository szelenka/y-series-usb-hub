/**
 * @file Animation.cpp
 * @brief Implementation of the Animation class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file contains the implementation of the Animation class which manages
 * all animation and interaction behaviors of the Y-Series USB Hub.
 * It handles motor control, sensor inputs, and coordinates with other
 * system components like the LED animations and audio playback.
 */

// System includes
#include <Arduino.h>

// Project includes
#include "Animation.h"
#include "../Logger/Logger.h"

Animation::Animation(EyeAnimation* eye, AudioPlayer* audio, const AnimationPins& pins)
    : m_eyeAnimation(eye),
      m_audioPlayer(audio),
      m_pins(pins),
      m_motorDirection(MotorDirection::Stop),
      m_lastLeftTurnTime(0),
      m_lastRightTurnTime(0),
      m_randomRotateTimer(0),
      m_inputSensorLeft(0),
      m_inputSensorRight(0),
      m_inputPIRSensor(0),
      m_lastPIRState(0),
      m_inputButtonRectangle(0),
      m_inputButtonCircle(0),
      m_lastPIRTimer(0),
      m_currentTime(0)
{
}

void Animation::update(const AnimationInputs& inputs)
{
    // Update sensor states
    setInputSensorLeft(inputs.sensorLeft);
    setInputSensorRight(inputs.sensorRight);
    setInputPIRSensor(inputs.pirSensor);
    setInputButtonRectangle(inputs.buttonRectangle);
    setInputButtonCircle(inputs.buttonCircle);
    setCurrentTime(inputs.currentTime);

    // Update eye animation time
    m_eyeAnimation->setCurrentTime(inputs.currentTime);
}

void Animation::rotate(uint8_t speed, MotorDirection direction)
{
    // Constrain speed to valid range
    const uint8_t safeSpeed = std::min(speed, AnimationConstants::kMaxMotorSpeed);

    // Apply minimum speed if moving
    const uint8_t effectiveSpeed = (direction != MotorDirection::Stop)
                                       ? std::max(safeSpeed, AnimationConstants::kMinSpeed)
                                       : 0;

    // Control motor based on direction
    switch (direction)
    {
        case MotorDirection::Right:
            analogWrite(m_pins.neckMotorIn2, effectiveSpeed);
            analogWrite(m_pins.neckMotorIn1, LOW);
            break;

        case MotorDirection::Left:
            analogWrite(m_pins.neckMotorIn1, effectiveSpeed);
            analogWrite(m_pins.neckMotorIn2, LOW);
            break;

        case MotorDirection::Stop:
        default:
            stop();
            return;  // Early return to avoid updating direction
    }

    // Update direction state if it has changed
    if (m_motorDirection != direction)
    {
        m_motorDirection = direction;
        Log.info("Motor direction changed to: %d", static_cast<int>(direction));
    }
}

void Animation::stop()
{
    // Only update if we're not already stopped
    if (m_motorDirection != MotorDirection::Stop)
    {
        // Set both motor control pins to LOW to stop the motor
        analogWrite(m_pins.neckMotorIn1, LOW);
        analogWrite(m_pins.neckMotorIn2, LOW);

        // Update motor state
        m_motorDirection = MotorDirection::Stop;
        Log.info("Motor stopped");
    }
}

void Animation::setRotationDirection()
{
    // Check limit sensors first - these take highest priority
    if (m_inputSensorLeft == LOW)
    {
            // Normal case: moving right and hit left sensor, reverse to right
            m_motorDirection = MotorDirection::Right;
            m_randomRotateTimer = m_currentTime + AnimationConstants::kMinDirectionTime;  // Give enough time to move away from the limit
            m_lastRightTurnTime = m_currentTime;
            return;
    }
    else if (m_inputSensorRight == LOW)
    {
            // Normal case: moving left and hit right sensor, reverse to left
            m_motorDirection = MotorDirection::Left;
            m_randomRotateTimer = m_currentTime + AnimationConstants::kMinDirectionTime;  // Give enough time to move away from the limit
            m_lastLeftTurnTime = m_currentTime;
            return;
    }

    // If no sensor is triggered, handle random direction changes
    if (m_randomRotateTimer == 0)
    {
        // Calculate time since last turn in each direction
        const uint32_t timeSinceLeft = m_currentTime - m_lastLeftTurnTime;
        const uint32_t timeSinceRight = m_currentTime - m_lastRightTurnTime;

        // Initialize biases with default values
        float leftBias = AnimationConstants::kNormalBias;
        float rightBias = AnimationConstants::kNormalBias;

        // Apply stronger bias based on time since last turn in each direction
        if (timeSinceLeft < AnimationConstants::kMinDirectionTime)
        {
            // Recent left turn, bias toward continuing left
            leftBias = AnimationConstants::kStrongBias;
            Log.debug("Strong left bias (recent turn)");
        }
        else if (timeSinceLeft > AnimationConstants::kMaxDirectionTime)
        {
            // It's been a long time since left turn, strongly prefer left
            rightBias = AnimationConstants::kStrongerBias;
            Log.debug("Strong right bias (long time since left turn)");
        }

        if (timeSinceRight < AnimationConstants::kMinDirectionTime)
        {
            // Recent right turn, bias toward continuing right
            rightBias = AnimationConstants::kStrongBias;
            Log.debug("Strong right bias (recent turn)");
        }
        else if (timeSinceRight > AnimationConstants::kMaxDirectionTime)
        {
            // It's been a long time since right turn, strongly prefer right
            leftBias = AnimationConstants::kStrongerBias;
            Log.debug("Strong left bias (long time since right turn)");
        }

        // Calculate total bias and make weighted random decision
        const float totalBias = leftBias + rightBias;
        const float randomValue = random(1000) / 1000.0f * totalBias;
        // Select direction based on weighted random value
        if (randomValue < leftBias)
        {
            m_motorDirection = MotorDirection::Left;
            m_lastLeftTurnTime = m_currentTime;
            Log.debug("Selected LEFT direction (%.2f/%.2f)", randomValue, leftBias);
        }
        else
        {
            m_motorDirection = MotorDirection::Right;
            m_lastRightTurnTime = m_currentTime;
            Log.debug("Selected RIGHT direction (%.2f/%.2f)", randomValue - leftBias, rightBias);
        }

        // Set timer for next direction change
        m_randomRotateTimer = m_currentTime + random(AnimationConstants::kMinRotateInterval,
                                                     AnimationConstants::kMaxRotateInterval);
        Log.debug("[Animation] Direction timer set for %dms", m_randomRotateTimer - m_currentTime);
    }

    // Check if it's time to change direction
    if (m_currentTime >= m_randomRotateTimer)
    {
        Log.info("[Animation] Direction timer expired");
        m_randomRotateTimer = 0;  // Will trigger direction change in next call
    }
}

// Using constants from AnimationConstants namespace

/**
 * @brief Controls the motor rotation based on sensor input and timing
 *
 * This method handles the main rotation logic, including:
 * - Motion detection via PIR sensor
 * - Speed ramping for smooth operation
 * - Inactivity timeout
 * - Sound effects for state changes
 */
void Animation::performRotate()
{
    // Handle PIR sensor state
    if (m_inputPIRSensor == HIGH)
    {
        // Motion detected
        digitalWrite(m_pins.domeLedGreen, HIGH);
        setRotationDirection();
        handlePirTriggered();

        // Apply movement based on current direction
        if (m_motorDirection != MotorDirection::Stop)
        {
            rotate(AnimationConstants::kMaxMotorSpeed, m_motorDirection);
        }
    }
    else
    {
        // No motion detected
        digitalWrite(m_pins.domeLedGreen, LOW);
        handlePirInactive();

        // Stop motor when no motion is detected
        if (m_motorDirection != MotorDirection::Stop)
        {
            stop();
        }
    }
}

/**
 * @brief Handles logic when PIR sensor is triggered
 */
void Animation::handlePirTriggered()
{
    // Check for rising edge of PIR sensor (LOW -> HIGH)
    if (m_lastPIRState == LOW)
    {
        // This is a new motion detection event
        Log.info("Motion detected, starting rotation");

        // Play sound effect if audio player is available
        if (m_audioPlayer != nullptr)
        {
            m_audioPlayer->play(4);  // Play sound effect with ID 4
        }

        // Reset timers for fresh movement
        m_randomRotateTimer = 0;
    }

    // Update PIR state and reset the inactivity timer
    m_lastPIRState = HIGH;
    m_lastPIRTimer = m_currentTime;

    // Calculate how long we've been moving in the current direction
    const bool isMovingLeft = (m_motorDirection == MotorDirection::Left);
    const uint32_t directionDuration =
        m_currentTime - (isMovingLeft ? m_lastLeftTurnTime : m_lastRightTurnTime);

    // If we've been going in one direction too long, force a direction change
    if (directionDuration > AnimationConstants::kMaxDirectionTime)
    {
        Log.debug("Forcing direction change after %lums", directionDuration);
        m_randomRotateTimer = 0;  // Trigger direction change on next update
    }

    // Calculate speed with bell curve biasing (slow at start/end, faster in middle)
    const float t = std::min(directionDuration, AnimationConstants::kSpeedRampTime) /
                    static_cast<float>(AnimationConstants::kSpeedRampTime);
    const float speedBias = expf(-12.0f * (t - 0.5f) * (t - 0.5f));
    const int biasedSpeed =
        AnimationConstants::kMinSpeed +
        static_cast<int>((AnimationConstants::kMaxMotorSpeed - AnimationConstants::kMinSpeed) *
                         speedBias);

    // Apply some random variation to the speed for more natural movement
    // Ensure we don't exceed maximum motor speed
    const int randomSpeed =
        random(AnimationConstants::kMinSpeed,
               std::min(biasedSpeed + 1, static_cast<int>(AnimationConstants::kMaxMotorSpeed)));

    rotate(static_cast<uint8_t>(randomSpeed), m_motorDirection);

    Log.debug("[Animation] Motor speed: %d (bias=%.2f, duration=%dms)", randomSpeed, speedBias,
              directionDuration);
}

/**
 * @brief Handles logic when PIR sensor is inactive
 */
void Animation::handlePirInactive()
{
    // Check for falling edge of PIR sensor (HIGH -> LOW)
    if (m_lastPIRState == HIGH)
    {
        Log.info("Motion no longer detected, starting inactivity timer");
    }

    // Update PIR state
    m_lastPIRState = LOW;

    // Check if we've been inactive too long and need to fully stop
    const uint32_t inactiveTime = m_currentTime - m_lastPIRTimer;
    if (inactiveTime >= AnimationConstants::kInactivityTimeout)
    {
        if (m_motorDirection != MotorDirection::Stop)
        {
            Log.info("Inactivity timeout reached, stopping motor");
            stop();
        }
    }
    else
    {
        Log.debug("Inactive for %lums/%lums", inactiveTime, AnimationConstants::kInactivityTimeout);
    }
}

void Animation::eyeBlink()
{
    // Update the eye animation based on the current mode
    if (m_inputButtonCircle == LOW)
    {
        m_eyeAnimation->updateRainbow();
    }
    else
    {
        m_eyeAnimation->updateDefault();
    }
}

void Animation::updateSound()
{
    // Check rectangle button for sound
    if (m_inputButtonRectangle == LOW && !m_audioPlayer->isPlaying())
    {
        m_audioPlayer->playRandomSound();
    }
    m_audioPlayer->update();
}
