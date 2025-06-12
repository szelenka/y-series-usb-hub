#include "Animation.h"

void Animation::update(const AnimationInputs& inputs)
{
    setInputSensorLeft(inputs.sensorLeft);
    setInputSensorRight(inputs.sensorRight);
    setInputPIRSensor(inputs.pirSensor);
    setInputButtonRectangle(inputs.buttonRectangle);
    setInputButtonCircle(inputs.buttonCircle);
    setCurrentTime(inputs.currentTime);
}

void Animation::rotate(uint8_t speed, MotorDirection direction)
{
    uint8_t safe_speed = constrain(speed, 0, 255);
    // Use std::clamp for C++17 and above for constraining values
    // uint8_t safe_speed = std::clamp(speed, static_cast<uint8_t>(0), static_cast<uint8_t>(255));
    if (direction == MotorDirection::Forward) {
        analogWrite(m_pins.neckMotorIn1, safe_speed);
        analogWrite(m_pins.neckMotorIn2, LOW);
    } else if (direction == MotorDirection::Backward) {
        analogWrite(m_pins.neckMotorIn2, safe_speed);
        analogWrite(m_pins.neckMotorIn1, LOW);
    } else {
        stop();
    }
}

void Animation::stop()
{
    analogWrite(m_pins.neckMotorIn1, LOW);
    analogWrite(m_pins.neckMotorIn2, LOW);
}

void Animation::setRotationDirection()
{
    // Rotate the motor if either sensor is triggered
    // the dome should _always_ rotate between these two
    if (m_inputSensorLeft == HIGH || m_inputSensorRight == HIGH) {
        m_motorDirection *= -1;
        m_randomRotateTimer = 0; // reset random rotation timer
        m_logger.print("Sensor triggered, reversing direction to %s", 
            m_motorDirection == MotorDirection::Forward ? "Forward" : "Backward");
    } else {
        // If neither sensor is tripped, rotate in current direction for a random interval
        if (m_randomRotateTimer == 0) {
            unsigned long timeSinceLeft = m_currentTime - m_lastLeftTurnTime;
            unsigned long timeSinceRight = m_currentTime - m_lastRightTurnTime;

            // Calculate bias: if recently turned in one direction, favor that direction
            float leftBias = 1.0f;
            float rightBias = 1.0f;

            if (timeSinceLeft < 200) {
                leftBias = 2.0f; // strong bias to keep turning left
            } else if (timeSinceLeft > 2000) {
                leftBias = 3.0f; // stronger bias to turn left if it's been a long time
            }

            if (timeSinceRight < 200) {
                rightBias = 2.0f; // strong bias to keep turning right
            } else if (timeSinceRight > 2000) {
                rightBias = 3.0f; // stronger bias to turn right if it's been a long time
            }

            // Weighted random direction
            float totalBias = leftBias + rightBias;
            float r = random(0, 1000) / 1000.0f * totalBias;
            if (r < leftBias) {
                m_motorDirection = MotorDirection::Forward;
                m_lastLeftTurnTime = m_currentTime;
                m_logger.print("Random direction chosen: Forward (bias=%.1f)", leftBias);
            } else {
                m_motorDirection = MotorDirection::Backward;
                m_lastRightTurnTime = m_currentTime;
                m_logger.print("Random direction chosen: Backward (bias=%.1f)", rightBias);
            }
            m_randomRotateTimer = m_currentTime + random(500, 1000);
            m_logger.print("Direction timer set for %dms", m_randomRotateTimer - m_currentTime);
        }
        if (m_currentTime >= m_randomRotateTimer) {
            m_logger.print("Direction timer expired");
            m_randomRotateTimer = 0;
        }
    }
}

void Animation::performRotate()
{
    // If PIR sensor is tripped, start motor and timer
    if (m_inputPIRSensor == HIGH) {
        // Play PIR sound when detected
        m_audioPlayer->play(0);  // PIR sound is at index 0
        
        unsigned long directionDuration = 0;
        if (m_motorDirection == MotorDirection::Forward) {
            directionDuration = m_currentTime - m_lastLeftTurnTime;
        } else {
            directionDuration = m_currentTime - m_lastRightTurnTime;
        }
        
        // Bias ramp-up: slow at start, faster in middle, slow again after long time
        // We'll use a simple bell curve (Gaussian-like) for biasing speed
        // Map duration (0 to 2s) to a value between 0 and 1, then apply bell curve
        float t = fmin(directionDuration, 2000UL) / 2000.0f; // 0.0 to 1.0
        float speedBias = exp(-12.0f * pow(t - 0.5f, 2)); // peak at t=0.5

        int minSpeed = 60;
        int maxSpeed = 255;
        int biasedSpeed = minSpeed + static_cast<int>((maxSpeed - minSpeed) * speedBias);

        int randomSpeed = random(minSpeed, biasedSpeed + 1);
        rotate(static_cast<uint8_t>(randomSpeed), m_motorDirection);
        m_lastPIRTimer = m_currentTime;
        m_logger.print("Motor speed set to %d (bias=%.2f, duration=%dms)", 
            randomSpeed, speedBias, directionDuration);
    } else {
        // If PIR sensor is not tripped, check if 30 seconds have passed since last trip
        if (m_currentTime - m_lastPIRTimer >= 30000) {
            // Play PIR sound when stopping
            m_audioPlayer->play(1);  // PIR sound is at index 1
            stop();
            m_logger.print("Stopping motor after 30s of inactivity");
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
