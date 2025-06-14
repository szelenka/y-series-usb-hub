#include "EyeAnimation.h"

EyeAnimation::EyeAnimation(Adafruit_NeoPixel* pixels) : m_pixels(pixels)
{
    m_rainbowIndex = 0;
    m_rainbowTimer = 0;
    m_defaultColor = 0x000080;  // Default: dark blue
    m_brightness = 255;
    m_currentTime = 0;

    // Initialize blink state
    m_isBlinking = false;
    m_blinkStartTime = 0;
    m_blinkDuration = 300;  // Default 300ms blink (longer for wave effect)
    m_blinkPhase = 0;
    m_blinkProgress = 0.0f;
    m_topPixel1 = 0;       // Default top pixels
    m_topPixel2 = 15;      // Default top pixels (opposite sides)
    m_blinkCount = 0;      // Number of blinks
    m_blinkEndTime = 0;    // Timestamp of last blink
    m_nextBlinkDelay = 0;  // Delay until next blink

    for (int i = 0; i < 16; i++)
    {
        m_pixelProgress[i] = 0.0f;
        m_pixelOrder[i] = i;  // Default order (will be updated by setTopPixels)
    }

    // Calculate initial pixel order
    calculatePixelOrder();

    // Initialize all pixels to off
    setAllPixelsColor(0);
}

void EyeAnimation::setDefaultColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_defaultColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void EyeAnimation::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
}

void EyeAnimation::updateRainbow()
{
    m_rainbowTimer = m_currentTime;

    // Calculate new color for each pixel
    for (uint16_t i = 0; i < m_pixels->numPixels(); i++)
    {
        uint8_t offset = (m_rainbowIndex + (i * 256 / m_pixels->numPixels())) % 256;
        setPixelColorWithBrightness(i, wheel(offset), m_brightness);
    }

    m_pixels->show();
    m_rainbowIndex = (m_rainbowIndex + 1) % 256;
    Log.debug("[EyeAnimation] Rainbow update: index=%d", m_rainbowIndex);

    // Update blink animation if active
    updateBlink();
    m_pixels->show();
}

void EyeAnimation::updateDefault()
{
    setAllPixelsColor(m_defaultColor);

    // Update blink animation if active
    updateBlink();
    m_pixels->show();
}

void EyeAnimation::setAllPixelsColor(uint32_t color)
{
    for (uint16_t i = 0; i < m_pixels->numPixels(); i++)
    {
        setPixelColorWithBrightness(i, color, m_brightness);
    }
}

void EyeAnimation::setPixelColorWithBrightness(uint16_t pixel, uint32_t color, uint8_t brightness)
{
    if (brightness == 255)
    {
        m_pixels->setPixelColor(pixel, color);
        return;
    }

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // Scale colors by brightness (0-255)
    r = (r * brightness) >> 8;
    g = (g * brightness) >> 8;
    b = (b * brightness) >> 8;

    uint32_t newColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    m_pixels->setPixelColor(pixel, newColor);
}

uint32_t EyeAnimation::wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
    {
        return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)0 << 8) | (pos * 3);
    }
    if (pos < 170)
    {
        pos -= 85;
        return ((uint32_t)0 << 16) | ((uint32_t)(pos * 3) << 8) | (255 - pos * 3);
    }
    pos -= 170;
    return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) | 0;
}

void EyeAnimation::setTopPixels(uint8_t topPixel1, uint8_t topPixel2)
{
    // Ensure pixels are within valid range (0-15)
    m_topPixel1 = topPixel1 % 16;
    m_topPixel2 = topPixel2 % 16;

    // Recalculate the pixel order
    calculatePixelOrder();
}

void EyeAnimation::calculatePixelOrder()
{
    // Clear the pixel order array
    for (int i = 0; i < 16; i++) {
        m_pixelOrder[i] = 0xFF;  // Initialize with invalid value
    }
    
    // Start with the two top pixels
    m_pixelOrder[0] = m_topPixel1;
    m_pixelOrder[1] = m_topPixel2;
    
    // Create a set to track used pixels
    bool used[16] = {false};
    used[m_topPixel1] = true;
    used[m_topPixel2] = true;
    
    // Current positions for each wave front
    int16_t leftPos = m_topPixel1;
    int16_t rightPos = m_topPixel2;
    
    // Fill in the remaining pixels in pairs, moving outward
    for (int i = 2; i < 16; ) {
        // Move left position counter-clockwise
        int16_t newLeftPos = (leftPos - 1 + 16) % 16;
        while (used[newLeftPos] && newLeftPos != rightPos) {
            newLeftPos = (newLeftPos - 1 + 16) % 16;
        }
        
        // Move right position clockwise
        int16_t newRightPos = (rightPos + 1) % 16;
        while (used[newRightPos] && newRightPos != newLeftPos) {
            newRightPos = (newRightPos + 1) % 16;
        }
        
        // If we've met in the middle, we're done
        if (newLeftPos == newRightPos) {
            if (!used[newLeftPos]) {
                m_pixelOrder[i++] = newLeftPos;
                used[newLeftPos] = true;
            }
            break;
        }
        
        // Add the new positions if they're not used
        if (!used[newLeftPos] && !used[newRightPos]) {
            m_pixelOrder[i++] = newLeftPos;
            used[newLeftPos] = true;
            
            if (i < 16) {
                m_pixelOrder[i++] = newRightPos;
                used[newRightPos] = true;
            }
        } else if (!used[newLeftPos]) {
            m_pixelOrder[i++] = newLeftPos;
            used[newLeftPos] = true;
        } else if (!used[newRightPos]) {
            m_pixelOrder[i++] = newRightPos;
            used[newRightPos] = true;
        }
        
        // Update positions for next iteration
        leftPos = newLeftPos;
        rightPos = newRightPos;
    }

    // Log the pixel order for debugging
    Log.info("[EyeAnimation] Pixel order: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
              m_pixelOrder[0], m_pixelOrder[1], m_pixelOrder[2], m_pixelOrder[3], m_pixelOrder[4],
              m_pixelOrder[5], m_pixelOrder[6], m_pixelOrder[7], m_pixelOrder[8], m_pixelOrder[9],
              m_pixelOrder[10], m_pixelOrder[11], m_pixelOrder[12], m_pixelOrder[13],
              m_pixelOrder[14], m_pixelOrder[15]);
}

void EyeAnimation::blink(unsigned long duration)
{
    if (m_isBlinking) {
        return;  // Don't interrupt an ongoing blink
    }

    // If blink count is 0, this is a manual blink
    if (m_blinkCount == 0) {
        m_blinkCount = 1;  // Set to 1 for a single blink
    }

    Log.debug("[EyeAnimation] Starting blink with duration %dms, m_blinkCount = %d", duration, m_blinkCount);

    // Initialize blink state
    m_isBlinking = true;
    m_blinkStartTime = m_currentTime;
    m_blinkDuration = duration > 0 ? duration : 300;  // Ensure non-zero duration
    m_blinkPhase = 1;                                 // Start with closing phase
    m_blinkProgress = 0.0f;
    m_blinkEndTime = m_blinkStartTime + m_blinkDuration;

    // Initialize all pixel progress to 0 (fully on)
    for (int i = 0; i < 16; i++) {
        m_pixelProgress[i] = 0.0f;
    }
}

void EyeAnimation::sequenceBlink()
{
    // If we're not currently blinking
    if (!m_isBlinking) {
        // If we have more blinks in the sequence, start the next one
        if (m_blinkCount > 0) {
            // Small delay between blinks in a sequence (100-200ms)
            static unsigned long lastBlinkEnd = 0;
            if (m_currentTime - lastBlinkEnd >= 150) {
                uint8_t duration = random(100, 400);
                blink(duration);  // 300ms per blink
                lastBlinkEnd = m_currentTime + duration;  // Update when this blink will end
            }
        } 
        // If no more blinks in sequence, schedule next sequence
        else if (m_nextBlinkDelay == 0) {
            // Set a random delay before next blink sequence (2-8 seconds)
            m_nextBlinkDelay = m_currentTime + random(2000, 8000);
        }
        // If it's time for a new blink sequence
        else if (m_currentTime >= m_nextBlinkDelay) {
            // 70% chance of single blink, 25% double blink, 5% triple blink
            uint8_t r = random(100);
            if (r < 70) {
                m_blinkCount = 1;
            } else if (r < 95) {
                m_blinkCount = 2;
            } else {
                m_blinkCount = 3;
            }
            m_nextBlinkDelay = 0;  // Reset for next sequence
        }
    }
}

bool EyeAnimation::updateBlink()
{
    sequenceBlink();
    if (!m_isBlinking) {
        return false;
    }

    unsigned long elapsed = m_currentTime - m_blinkStartTime;
    m_blinkProgress = (float)elapsed / (m_blinkDuration / 2.0f);  // Progress for current phase

    if (m_blinkProgress >= 1.0f) {
        // Phase complete
        if (m_blinkPhase == 1) {
            // Switch to opening phase
            m_blinkPhase = 2;
            m_blinkStartTime = m_currentTime;
            m_blinkProgress = 0.0f;
            m_blinkEndTime = m_blinkStartTime + m_blinkDuration;
        } else {
            // Blink complete
            m_isBlinking = false;
            if (m_blinkCount > 0) {
                m_blinkCount--;
            }
            return false;
        }
    }

    // Total number of pixel pairs (center pair + 7 outer pairs)
    const int numPairs = 4;
    float pairProgress;
    // Update each pixel pair
    if (m_blinkPhase == 1) { // Closing phase
        pairProgress = m_blinkProgress * numPairs;  // Scales 0.0 to numPairs
    } else {  // Opening phase
        pairProgress = (1.0f - m_blinkProgress) * numPairs;  // Scales 0.0 to numPairs
    }
    // Update each pair
    for (int pair = 0; pair < numPairs; pair++) {
        // Get the two pixels in this pair
        int idx1 = pair * 2;
        int idx2 = pair * 2 + 1;
        // mirror the pixels to the other side of the eye
        int idx3 = 15 - idx1; // numPixels - 1
        int idx4 = 15 - idx2; // numPixels - 1
        // 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
        // 0,1 + 15,14
        // 2,3 + 13,12
        // 4,5 + 11,10
        // 6,7 + 9,8
        // 5,4,6,3,7,2,8,1,9,0,10,15,11,14,12,13
        // 5,4 + 13,12
        // 6,3 + 14,11
        // 7,2 + 15,10
        // 8,1 + 16,9
        
        uint8_t pixel1 = m_pixelOrder[idx1];
        uint8_t pixel2 = m_pixelOrder[idx2];
        uint8_t pixel3 = m_pixelOrder[idx3];
        uint8_t pixel4 = m_pixelOrder[idx4];
    
        // Calculate progress for this pair (0.0 to 1.0)
        float localProgress = (pairProgress - pair) / 1.0f;
        localProgress = constrain(localProgress, 0.0f, 1.0f);
        
        // Update both pixels in the pair
        m_pixelProgress[pixel1] = localProgress;
        m_pixelProgress[pixel2] = localProgress;
        m_pixelProgress[pixel3] = localProgress;
        m_pixelProgress[pixel4] = localProgress;
    }

    // Update all pixels based on their current progress
    for (int i = 0; i < 16; i++) {
        uint8_t pixelIndex = m_pixelOrder[i];
        
        // Calculate brightness based on phase and progress
        float brightness = 1.0f - m_pixelProgress[pixelIndex];
        
        // Get the pixel's current color and apply brightness
        uint32_t color = m_pixels->getPixelColor(pixelIndex);
        setPixelColorWithBrightness(pixelIndex, color, brightness * 255);
    }
    // set the center pixel to off when all others are off
    int centerPixelRef = (m_topPixel1 + numPairs) % 16;
    setPixelColorWithBrightness(16, m_pixels->getPixelColor(16), 1.0f - m_pixelProgress[centerPixelRef] * 255);

    return true;
}
