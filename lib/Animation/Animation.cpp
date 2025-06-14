/**
 * @file Animation.cpp
 * @brief Implementation of the Animation class for Y-Series USB Hub
 */

#include <Arduino.h>

#include "Animation.h"
#include <Logger.h>

// Use constants from Animation.h

void Animation::update(const AnimationInputs& inputs)
{
    setInputSensorLeft(inputs.sensorLeft);
    setInputSensorRight(inputs.sensorRight);
    setInputPIRSensor(inputs.pirSensor);
    setInputButtonRectangle(inputs.buttonRectangle);
    setInputButtonCircle(inputs.buttonCircle);
    setCurrentTime(inputs.currentTime);
    m_eyeAnimation->setCurrentTime(inputs.currentTime);
}

void Animation::rotate(uint8_t speed, MotorDirection direction)
{
    // Constrain speed to valid range
    const uint8_t safe_speed = std::min(speed, AnimationConstants::kMaxMotorSpeed);

    switch (direction)
    {
        case MotorDirection::Forward:
            analogWrite(m_pins.neckMotorIn1, safe_speed);
            analogWrite(m_pins.neckMotorIn2, LOW);
            break;

        case MotorDirection::Backward:
            analogWrite(m_pins.neckMotorIn2, safe_speed);
            analogWrite(m_pins.neckMotorIn1, LOW);
            break;

        case MotorDirection::Stop:
        default:
            stop();
            break;
    }
}

void Animation::stop()
{
    // Ensure both motor control pins are set to LOW to stop the motor
    analogWrite(m_pins.neckMotorIn1, LOW);
    analogWrite(m_pins.neckMotorIn2, LOW);

    // Update motor state
    m_motorDirection = MotorDirection::Stop;
}

// Using direction bias timing constants from Animation.h

/**
 * @brief Sets the rotation direction based on sensor input and timing
 *
 * This method implements the core logic for determining the motor's rotation direction.
 * It considers sensor inputs and timing to create natural-looking movement patterns.
 */
void Animation::setRotationDirection()
{
    // Check if either limit sensor is triggered
    if (m_inputSensorLeft == HIGH || m_inputSensorRight == HIGH)
    {
        // Reverse direction when hitting a limit
        m_motorDirection *= -1;
        m_randomRotateTimer = 0;  // Reset the rotation timer

        Log.debug("[Animation] Hall effect sensor triggered, direction: %s",
                  m_motorDirection == MotorDirection::Forward ? "Forward" : "Backward");
        return;
    }

    // If no sensor is triggered, handle random direction changes
    if (m_randomRotateTimer == 0)
    {
        // Random direction selection with bias based on time since last turn
        uint32_t timeSinceLeft = m_currentTime - m_lastLeftTurnTime;
        uint32_t timeSinceRight = m_currentTime - m_lastRightTurnTime;

        // Calculate direction bias based on time since last turn in each direction
        float leftBias = AnimationConstants::kNormalBias;
        float rightBias = AnimationConstants::kNormalBias;

        // Apply stronger bias if we've been turning in one direction for too long
        if (timeSinceLeft < AnimationConstants::kMinDirectionTime)
        {
            leftBias = AnimationConstants::kStrongBias;  // Keep turning left
        }
        else if (timeSinceLeft > AnimationConstants::kMaxDirectionTime)
        {
            leftBias = AnimationConstants::kStrongerBias;  // Strong preference to turn left
        }

        if (timeSinceRight < AnimationConstants::kMinDirectionTime)
        {
            rightBias = AnimationConstants::kStrongBias;  // Keep turning right
        }
        else if (timeSinceRight > AnimationConstants::kMaxDirectionTime)
        {
            rightBias = AnimationConstants::kStrongerBias;  // Strong preference to turn right
        }

        // Weighted random direction selection
        float total = leftBias + rightBias;
        float randomValue = random(0, 1000) / 1000.0f * total;

        if (randomValue < leftBias)
        {
            m_motorDirection = MotorDirection::Forward;
            m_lastLeftTurnTime = m_currentTime;
            Log.debug("[Animation] Random direction chosen: Forward (bias=%.1f)", leftBias);
        }
        else
        {
            m_motorDirection = MotorDirection::Backward;
            m_lastRightTurnTime = m_currentTime;
            Log.debug("[Animation] Random direction chosen: Backward (bias=%.1f)", rightBias);
        }

        // Set timer for next direction change
        m_randomRotateTimer = m_currentTime + random(AnimationConstants::kMinRotateInterval,
                                                     AnimationConstants::kMaxRotateInterval);
        Log.debug("[Animation] Direction timer set for %dms", m_randomRotateTimer - m_currentTime);
    }

    // Check if it's time to change direction
    if (m_currentTime >= m_randomRotateTimer)
    {
        Log.debug("[Animation] Direction timer expired");
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
        handlePirTriggered();
    }
    else
    {
        handlePirInactive();
    }
}

/**
 * @brief Handles logic when PIR sensor is triggered
 */
void Animation::handlePirTriggered()
{
    // Play sound effect when motion is first detected
    if (m_lastPIRState != HIGH)
    {
        m_audioPlayer->play(4);
        Log.info("[Animation] Motion detected, starting rotation");
    }

    // Reset the inactivity timer
    m_lastPIRState = HIGH;
    m_lastPIRTimer = m_currentTime;

    // Calculate direction duration for speed biasing
    const uint32_t directionDuration = (m_motorDirection == MotorDirection::Forward)
                                           ? m_currentTime - m_lastLeftTurnTime
                                           : m_currentTime - m_lastRightTurnTime;

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

    // Set the motor direction and speed
    if (m_motorDirection == MotorDirection::Stop)
    {
        setRotationDirection();
    }

    rotate(static_cast<uint8_t>(randomSpeed), m_motorDirection);

    Log.debug("[Animation] Motor speed: %d (bias=%.2f, duration=%dms)", randomSpeed, speedBias,
              directionDuration);
}

/**
 * @brief Handles logic when PIR sensor is inactive
 */
void Animation::handlePirInactive()
{
    // Update state if we just transitioned from active to inactive
    if (m_lastPIRState == HIGH)
    {
        Log.info("[Animation] Motion no longer detected, starting inactivity timer");
    }
    m_lastPIRState = LOW;

    // Check if we've been inactive too long
    if (m_currentTime - m_lastPIRTimer >= AnimationConstants::kInactivityTimeout)
    {
        // Only stop if we're not already stopped
        if (m_motorDirection != MotorDirection::Stop)
        {
            m_audioPlayer->play(10);
            stop();
            Log.info("[Animation] Stopping motor after %dms of inactivity",
                     AnimationConstants::kInactivityTimeout);
        }
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
