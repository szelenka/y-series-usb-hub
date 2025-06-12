/**
 * @file Animation.cpp
 * @brief Implementation of the Animation class for Y-Series USB Hub
 */

#include "Animation.h"

#include <algorithm>

// Use constants from Animation.h

void Animation::update(const AnimationInputs& inputs) {
    setInputSensorLeft(inputs.sensorLeft);
    setInputSensorRight(inputs.sensorRight);
    setInputPIRSensor(inputs.pirSensor);
    setInputButtonRectangle(inputs.buttonRectangle);
    setInputButtonCircle(inputs.buttonCircle);
    setCurrentTime(inputs.currentTime);
}

void Animation::rotate(uint8_t speed, MotorDirection direction) {
    // Constrain speed to valid range
    const uint8_t safe_speed = std::min(speed, AnimationConstants::kMaxMotorSpeed);
    
    switch (direction) {
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

void Animation::stop() {
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
void Animation::setRotationDirection() {
    // Check if either limit sensor is triggered
    if (m_inputSensorLeft == HIGH || m_inputSensorRight == HIGH) {
        // Reverse direction when hitting a limit
        m_motorDirection *= -1;
        m_randomRotateTimer = 0;  // Reset the rotation timer
        
        m_logger.print("Sensor triggered, reversing direction to %s",
                       m_motorDirection == MotorDirection::Forward ? "Forward" : "Backward");
        return;
    }

    // If no sensor is triggered, handle random direction changes
    if (m_randomRotateTimer == 0) {
        const uint32_t timeSinceLeft = m_currentTime - m_lastLeftTurnTime;
        const uint32_t timeSinceRight = m_currentTime - m_lastRightTurnTime;

        // Calculate direction bias based on time since last turn in each direction
        float leftBias = AnimationConstants::kNormalBias;
        float rightBias = AnimationConstants::kNormalBias;

        // Apply stronger bias if we've been turning in one direction for too long
        if (timeSinceLeft < AnimationConstants::kMinDirectionTime) {
            leftBias = AnimationConstants::kStrongBias;  // Keep turning left
        } else if (timeSinceLeft > AnimationConstants::kMaxDirectionTime) {
            leftBias = AnimationConstants::kStrongerBias;  // Strong preference to turn left
        }

        if (timeSinceRight < AnimationConstants::kMinDirectionTime) {
            rightBias = AnimationConstants::kStrongBias;  // Keep turning right
        } else if (timeSinceRight > AnimationConstants::kMaxDirectionTime) {
            rightBias = AnimationConstants::kStrongerBias;  // Strong preference to turn right
        }

        // Weighted random direction selection
        const float totalBias = leftBias + rightBias;
        const float randomValue = random(0, 1000) / 1000.0f * totalBias;
        
        if (randomValue < leftBias) {
            m_motorDirection = MotorDirection::Forward;
            m_lastLeftTurnTime = m_currentTime;
            m_logger.print("Random direction chosen: Forward (bias=%.1f)", leftBias);
        } else {
            m_motorDirection = MotorDirection::Backward;
            m_lastRightTurnTime = m_currentTime;
            m_logger.print("Random direction chosen: Backward (bias=%.1f)", rightBias);
        }

        // Set timer for next direction change
        m_randomRotateTimer = m_currentTime + 
            random(AnimationConstants::kMinRotateInterval, AnimationConstants::kMaxRotateInterval);
        m_logger.print("Direction timer set for %dms", m_randomRotateTimer - m_currentTime);
    }

    // Check if it's time to change direction
    if (m_currentTime >= m_randomRotateTimer) {
        m_logger.print("Direction timer expired");
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
void Animation::performRotate() {
    // Handle PIR sensor state
    if (m_inputPIRSensor == HIGH) {
        handlePirTriggered();
    } else {
        handlePirInactive();
    }
}

/**
 * @brief Handles logic when PIR sensor is triggered
 */
void Animation::handlePirTriggered() {
    // Play sound effect when motion is first detected
    if (m_lastPIRState != HIGH) {
        m_audioPlayer->play(0);  // Play motion detected sound
        m_logger.print("Motion detected, starting rotation");
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
    const int biasedSpeed = AnimationConstants::kMinSpeed + 
                           static_cast<int>((AnimationConstants::kMaxMotorSpeed - AnimationConstants::kMinSpeed) * speedBias);
    
    // Apply some random variation to the speed for more natural movement
    // Ensure we don't exceed maximum motor speed
    const int randomSpeed = random(
        AnimationConstants::kMinSpeed, 
        std::min(biasedSpeed + 1, static_cast<int>(AnimationConstants::kMaxMotorSpeed))
    );
    
    // Set the motor direction and speed
    if (m_motorDirection == MotorDirection::Stop) {
        setRotationDirection();
    }
    
    rotate(static_cast<uint8_t>(randomSpeed), m_motorDirection);
    
    m_logger.print("Motor speed: %d (bias=%.2f, duration=%dms)", 
                   randomSpeed, speedBias, directionDuration);
}

/**
 * @brief Handles logic when PIR sensor is inactive
 */
void Animation::handlePirInactive() {
    // Update state if we just transitioned from active to inactive
    if (m_lastPIRState == HIGH) {
        m_logger.print("Motion no longer detected, starting inactivity timer");
    }
    m_lastPIRState = LOW;
    
    // Check if we've been inactive too long
    if (m_currentTime - m_lastPIRTimer >= AnimationConstants::kInactivityTimeout) {
        // Only stop if we're not already stopped
        if (m_motorDirection != MotorDirection::Stop) {
            m_audioPlayer->play(1);  // Play motion stopped sound
            stop();
            m_logger.print("Stopping motor after %dms of inactivity", 
                           AnimationConstants::kInactivityTimeout);
        }
    }
}

void Animation::eyeBlink()
{
    if (m_inputButtonCircle == HIGH) {
        if (!m_isRainbowActive) {
            m_rainbowIndex = 0;
            m_rainbowTimer = m_currentTime;
            m_logger.print("Starting rainbow animation");
        }
        m_isRainbowActive = true;
    } else {
        m_isRainbowActive = false;
        m_pixels->clear();
        m_pixels->show();
    }
}

uint32_t Animation::wheel(uint8_t pos)
{
    // Standard RGB color wheel: 0=red, 85=green, 170=blue, 255=red
    uint8_t r, g, b;
    if (pos < 85) {
        r = 255 - pos * 3;
        g = pos * 3;
        b = 0;
    } else if (pos < 170) {
        pos -= 85;
        r = 0;
        g = 255 - pos * 3;
        b = pos * 3;
    } else {
        pos -= 170;
        r = pos * 3;
        g = 0;
        b = 255 - pos * 3;
    }
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void Animation::updateRainbow() 
{
    if (!m_isRainbowActive) {
        return;
    }

    if (m_currentTime - m_rainbowTimer >= 20) {  // Update every 20ms for smooth animation
        m_rainbowTimer = m_currentTime;
        
        // Calculate new color for each pixel
        for (uint16_t i = 0; i < m_pixels->numPixels(); i++) {
            uint8_t offset = (m_rainbowIndex + (i * 256 / m_pixels->numPixels())) % 256;
            m_pixels->setPixelColor(i, wheel(offset));
        }
        
        m_pixels->show();
        m_rainbowIndex = (m_rainbowIndex + 1) % 256;
        m_logger.print("Rainbow update: index=%d", m_rainbowIndex);
    }
}

void Animation::updateSound() {
    // Check rectangle button for sound
    if (m_inputButtonRectangle == HIGH && !m_audioPlayer->isPlaying()) {
        m_audioPlayer->playRandomSound();
    }
    m_audioPlayer->update();
}   
