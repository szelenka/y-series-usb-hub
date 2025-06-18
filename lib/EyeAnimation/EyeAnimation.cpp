/**
 * @file EyeAnimation.cpp
 * @brief Implementation of the EyeAnimation class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file implements the EyeAnimation class which manages LED animations for
 * the Y-Series USB Hub's eye display. It handles all the low-level details of
 * NeoPixel control while providing smooth, visually appealing eye animations.
 */

#include "EyeAnimation.h"

/**
 * @brief Construct a new EyeAnimation object
 *
 * @param[in] pixels Pointer to the Adafruit_NeoPixel instance
 */
EyeAnimation::EyeAnimation(Adafruit_NeoPixel* pixels)
    : m_pixels(pixels),
      m_rainbowIndex(0),
      m_rainbowTimer(0),
      m_activeColor(EyeAnimationConstants::COLOR_BLUE),
      m_brightness(EyeAnimationConstants::DEFAULT_BRIGHTNESS),
      m_currentTime(0),
      m_isBlinking(false),
      m_blinkStartTime(0),
      m_blinkDuration(EyeAnimationConstants::DEFAULT_BLINK_DURATION),
      m_blinkEndTime(0),
      m_blinkPhase(0),
      m_blinkProgress(0.0f),
      m_topPixel1(0),
      m_topPixel2(EyeAnimationConstants::NUM_PIXELS_IN_RING - 1),
      m_nextBlinkDelay(0),
      m_blinkCount(0),
      m_lastColorChangeTime(0),
      m_isSleeping(false)
{
    // Initialize pixel progress and order arrays
    for (uint16_t i = 0; i < EyeAnimationConstants::NUM_PIXELS_IN_RING; i++)
    {
        m_pixelProgress[i] = 0.0f;
        m_pixelOrder[i] = i;  // Default order (will be updated by setTopPixels)
    }

    // Calculate initial pixel order
    calculatePixelOrder();

    // Initialize all pixels to off
    setAllPixelsColor(0);
}

/**
 * @brief Update the eyes with a rainbow animation effect
 *
 * @note This should be called regularly from the main loop
 */
void EyeAnimation::updateRainbowColor()
{
    if (!m_pixels)
    {
        return;
    }
    m_isSleeping = false;

    m_rainbowTimer = m_currentTime;

    // Calculate new color for each pixel
    for (uint16_t i = 0; i < m_pixels->numPixels(); i++)
    {
        // Distribute the color wheel across all pixels
        uint8_t offset = (m_rainbowIndex + (i * 256 / m_pixels->numPixels())) % 256;
        setPixelColorWithBrightness(i, wheel(offset), m_brightness);
    }

    // Move to the next color in the rainbow
    m_rainbowIndex = (m_rainbowIndex + 1) % 256;

    // Update blink animation if active
    updateBlink();

    // Final update to show any blink changes
    show();
}

/**
 * @brief Update the eyes with the active solid color
 *
 * @note This should be called regularly from the main loop
 */
void EyeAnimation::updateActiveColor()
{
    if (!m_pixels)
    {
        return;
    }
    m_isSleeping = false;

    // Set all pixels to the active color
    setAllPixelsColor(m_activeColor);

    // Update blink animation if active
    updateBlink();

    // Update the display
    show();
}

/**
 * @brief Put the eye animation to sleep
 */
void EyeAnimation::sleep()
{
    if (!m_pixels || m_isSleeping)
    {
        return;
    }

    for (uint16_t i = 0; i < m_pixels->numPixels(); i++)
    {
        m_pixels->setPixelColor(i, 0);
    }
    show();
    m_isSleeping = true;
}

/**
 * @brief Set all pixels to the specified color
 *
 * @param[in] color 32-bit color value (0x00RRGGBB)
 */
void EyeAnimation::setAllPixelsColor(uint32_t color)
{
    if (!m_pixels)
    {
        return;
    }

    for (uint16_t i = 0; i < EyeAnimationConstants::NUM_PIXELS_IN_RING; i++)
    {
        setPixelColorWithBrightness(i, color, m_brightness);
    }

    // The colors blue/green are too bright where the center pixel gives off a white glow
    // alter the colors to provide a more intentional deviation for the center of the eye
    if (EyeAnimationConstants::COLOR_BLUE == color)
    {
        setPixelColorWithBrightness(EyeAnimationConstants::NUM_PIXELS_IN_RING, 0x0000FF,
                                    m_brightness);
    }
    else
    {
        setPixelColorWithBrightness(EyeAnimationConstants::NUM_PIXELS_IN_RING, 0x00FF00,
                                    m_brightness);
    }
}

/**
 * @brief Set a single pixel's color with brightness adjustment
 *
 * @param[in] pixel Pixel index (0 to numPixels-1)
 * @param[in] color 32-bit color value (0x00RRGGBB)
 * @param[in] brightness Brightness value (0-255)
 *
 * @note If brightness is 255, the color is set directly for better performance
 */
void EyeAnimation::setPixelColorWithBrightness(uint16_t pixel, uint32_t color, uint8_t brightness)
{
    if (!m_pixels || pixel >= m_pixels->numPixels())
    {
        return;  // Safety check
    }

    // Fast path for full brightness
    if (brightness == 255)
    {
        m_pixels->setPixelColor(pixel, color);
        return;
    }

    // Extract RGB components
    uint8_t r = static_cast<uint8_t>((color >> 16) & 0xFF);
    uint8_t g = static_cast<uint8_t>((color >> 8) & 0xFF);
    uint8_t b = static_cast<uint8_t>(color & 0xFF);

    // Scale colors by brightness (using fixed-point math for efficiency)
    r = static_cast<uint8_t>((r * brightness) >> 8);
    g = static_cast<uint8_t>((g * brightness) >> 8);
    b = static_cast<uint8_t>((b * brightness) >> 8);

    // Combine back into 32-bit color
    uint32_t newColor = (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | b;

    m_pixels->setPixelColor(pixel, newColor);
}

/**
 * @brief Update the display
 */
void EyeAnimation::show()
{
    if (!m_pixels || m_isSleeping)
    {
        return;
    }
    m_pixels->show();
}

/**
 * @brief Generate a color from a position on the color wheel
 *
 * @param[in] pos Position on the color wheel (0-255)
 * @return uint32_t Color value (0x00RRGGBB)
 *
 * @note This creates a smooth color transition through the rainbow
 */
uint32_t EyeAnimation::wheel(uint8_t pos)
{
    pos = 255 - pos;  // Reverse direction for better color progression

    // Calculate color based on position in the color wheel
    if (pos < 85)
    {
        // Red to Green transition (decreasing red, increasing green)
        return (static_cast<uint32_t>(255 - pos * 3) << 16) | (static_cast<uint32_t>(pos * 3) << 8);
    }
    else if (pos < 170)
    {
        // Green to Blue transition (decreasing green, increasing blue)
        pos -= 85;
        return (static_cast<uint32_t>(pos * 3) << 8) | (static_cast<uint32_t>(255 - pos * 3));
    }
    else
    {
        // Blue to Red transition (decreasing blue, increasing red)
        pos -= 170;
        return (static_cast<uint32_t>(255 - pos * 3) << 16) | (static_cast<uint32_t>(pos * 3));
    }
}

/**
 * @brief Set the top pixels for blink animation
 *
 * @param[in] topPixel1 First top pixel index (0-15)
 * @param[in] topPixel2 Second top pixel index (0-15)
 *
 * @note The blink animation will start from these pixels and move outward
 */
void EyeAnimation::setTopPixels(uint8_t topPixel1, uint8_t topPixel2)
{
    // Ensure pixels are within valid range (0-15)
    m_topPixel1 = topPixel1 % EyeAnimationConstants::NUM_PIXELS_IN_RING;
    m_topPixel2 = topPixel2 % EyeAnimationConstants::NUM_PIXELS_IN_RING;

    Log.debug("Set top pixels to %d and %d", m_topPixel1, m_topPixel2);

    // Recalculate the animation order for the blink effect
    calculatePixelOrder();
}

/**
 * @brief Calculate the order in which pixels should animate during a blink
 *
 * @note This creates a wave-like animation that spreads out from the top pixels
 */
void EyeAnimation::calculatePixelOrder()
{
    // Clear the pixel order array with invalid values
    for (uint16_t i = 0; i < EyeAnimationConstants::NUM_PIXELS_IN_RING; i++)
    {
        m_pixelOrder[i] = 0xFF;
    }

    // Start with the two top pixels
    m_pixelOrder[0] = m_topPixel1;
    m_pixelOrder[1] = m_topPixel2;

    // Track which pixels have been assigned an order
    bool used[EyeAnimationConstants::NUM_PIXELS_IN_RING] = {false};
    used[m_topPixel1] = true;
    used[m_topPixel2] = true;

    // Current positions for the animation wave front
    int16_t leftPos = m_topPixel1;
    int16_t rightPos = m_topPixel2;

    // Fill in the remaining pixels in wave order
    for (uint16_t i = 2; i < EyeAnimationConstants::NUM_PIXELS_IN_RING;)
    {
        // Move left position counter-clockwise
        int16_t newLeftPos = (leftPos - 1 + EyeAnimationConstants::NUM_PIXELS_IN_RING) %
                             EyeAnimationConstants::NUM_PIXELS_IN_RING;
        while (used[newLeftPos] && newLeftPos != rightPos)
        {
            newLeftPos = (newLeftPos - 1 + EyeAnimationConstants::NUM_PIXELS_IN_RING) %
                         EyeAnimationConstants::NUM_PIXELS_IN_RING;
        }

        // Move right position clockwise
        int16_t newRightPos = (rightPos + 1) % EyeAnimationConstants::NUM_PIXELS_IN_RING;
        while (used[newRightPos] && newRightPos != newLeftPos)
        {
            newRightPos = (newRightPos + 1) % EyeAnimationConstants::NUM_PIXELS_IN_RING;
        }

        // If we've met in the middle, we're done
        if (newLeftPos == newRightPos)
        {
            if (!used[newLeftPos])
            {
                m_pixelOrder[i++] = newLeftPos;
                used[newLeftPos] = true;
            }
            break;
        }

        // Add the new positions if they're not used
        if (!used[newLeftPos] && !used[newRightPos])
        {
            m_pixelOrder[i++] = newLeftPos;
            used[newLeftPos] = true;

            if (i < EyeAnimationConstants::NUM_PIXELS_IN_RING)
            {
                m_pixelOrder[i++] = newRightPos;
                used[newRightPos] = true;
            }
        }
        else if (!used[newLeftPos])
        {
            m_pixelOrder[i++] = newLeftPos;
            used[newLeftPos] = true;
        }
        else if (!used[newRightPos])
        {
            m_pixelOrder[i++] = newRightPos;
            used[newRightPos] = true;
        }

        // Update positions for next iteration
        leftPos = newLeftPos;
        rightPos = newRightPos;
    }

    // Log the pixel order for debugging
    Log.debug("Pixel animation order: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
              m_pixelOrder[0], m_pixelOrder[1], m_pixelOrder[2], m_pixelOrder[3], m_pixelOrder[4],
              m_pixelOrder[5], m_pixelOrder[6], m_pixelOrder[7], m_pixelOrder[8], m_pixelOrder[9],
              m_pixelOrder[10], m_pixelOrder[11], m_pixelOrder[12], m_pixelOrder[13],
              m_pixelOrder[14], m_pixelOrder[15]);
}

/**
 * @brief Start a blink animation
 *
 * @param[in] duration Total duration of the blink in milliseconds
 *
 * @note If a blink is already in progress, this call will be ignored
 */
void EyeAnimation::blink(unsigned long duration)
{
    if (m_isBlinking)
    {
        Log.debug("Blink already in progress, ignoring new blink request");
        return;  // Don't interrupt current blink
    }

    m_isBlinking = true;
    m_blinkStartTime = m_currentTime;
    m_blinkDuration = duration > 0 ? duration : EyeAnimationConstants::DEFAULT_BLINK_DURATION;
    m_blinkPhase = 1;  // Start closing
    m_blinkProgress = 0.0f;
    m_blinkEndTime = m_blinkStartTime + m_blinkDuration;

    // Initialize all pixel progress to 0 (fully on)
    for (uint16_t i = 0; i < EyeAnimationConstants::NUM_PIXELS_IN_RING; i++)
    {
        m_pixelProgress[i] = 0.0f;
    }

    Log.debug("Started blink animation for %lu ms", m_blinkDuration);
}

/**
 * @brief Start a sequence of blinks (like a double or triple blink)
 *
 * @note The number of blinks is randomly chosen between 2 and 4
 */
void EyeAnimation::sequenceBlink()
{
    // If we're not currently blinking
    if (!m_isBlinking)
    {
        // If we have more blinks in the sequence, start the next one
        if (m_blinkCount > 0)
        {
            // Small delay between blinks in a sequence (100-200ms)
            static unsigned long lastBlinkEnd = 0;
            if (m_currentTime - lastBlinkEnd >= 200)
            {
                uint8_t duration = random(200, 400);
                blink(duration);
                lastBlinkEnd = m_currentTime + duration;  // Update when this blink will end
            }
        }
        // If no more blinks in sequence, schedule next sequence
        else if (m_nextBlinkDelay == 0)
        {
            // Set a random delay before next blink sequence (2-8 seconds)
            m_nextBlinkDelay = m_currentTime + random(2000, 8000);
        }
        // If it's time for a new blink sequence
        else if (m_currentTime >= m_nextBlinkDelay)
        {
            // 70% chance of single blink, 25% double blink, 5% triple blink
            uint8_t r = random(100);
            if (r < 70)
            {
                m_blinkCount = 1;
            }
            else if (r < 95)
            {
                m_blinkCount = 2;
            }
            else
            {
                m_blinkCount = 3;
            }
            m_nextBlinkDelay = 0;  // Reset for next sequence
        }
    }
}

/**
 * @brief Update the blink animation state
 *
 * @return true if a blink is in progress, false otherwise
 *
 * @note This should be called regularly from the main loop to update the animation
 */
bool EyeAnimation::updateBlink()
{
    if (!m_pixels)
    {
        return false;  // Safety check
    }

    // Handle blink sequence if needed
    sequenceBlink();

    // If no blink is in progress, nothing to do
    if (!m_isBlinking)
    {
        return false;
    }

    // Calculate progress through current blink phase (0.0 to 1.0)
    unsigned long elapsed = m_currentTime - m_blinkStartTime;
    m_blinkProgress = static_cast<float>(elapsed) / (m_blinkDuration / 2.0f);

    // Handle phase completion
    if (m_blinkProgress >= 1.0f)
    {
        if (m_blinkPhase == 1)
        {
            // Switch from closing to opening phase
            m_blinkPhase = 2;
            m_blinkStartTime = m_currentTime;
            m_blinkProgress = 0.0f;
            m_blinkEndTime = m_blinkStartTime + m_blinkDuration;
            Log.debug("Blink: Starting opening phase");
        }
        else
        {
            // Blink complete
            m_isBlinking = false;
            if (m_blinkCount > 0)
            {
                m_blinkCount--;
                Log.debug("Blink: Complete, %d blinks remaining", m_blinkCount);
            }
            else
            {
                Log.debug("Blink: Sequence complete");
            }
            return false;
        }
    }

    // Calculate animation progress for each ring of pixels
    const uint8_t numPairs = 4;  // Number of concentric rings of pixels
    float ringProgress;

    // Calculate progress for current phase
    if (m_blinkPhase == 1)
    {                                               // Closing phase
        ringProgress = m_blinkProgress * numPairs;  // Scales 0.0 to numPairs
    }
    else
    {                                                        // Opening phase
        ringProgress = (1.0f - m_blinkProgress) * numPairs;  // Scales 0.0 to numPairs
    }

    // Update progress for each ring of pixels
    for (uint8_t ring = 0; ring < numPairs; ring++)
    {
        // Get the pixels in this ring (4 pixels per ring: top1, top2, bottom1, bottom2)
        uint8_t idx1 = ring * 2;
        uint8_t idx2 = idx1 + 1;
        uint8_t idx3 = 15 - idx1;  // Mirror on the other side
        uint8_t idx4 = 15 - idx2;  // Mirror on the other side
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

        // Get the actual pixel indices from our animation order
        uint8_t pixel1 = m_pixelOrder[idx1];
        uint8_t pixel2 = m_pixelOrder[idx2];
        uint8_t pixel3 = m_pixelOrder[idx3];
        uint8_t pixel4 = m_pixelOrder[idx4];

        // Calculate progress for this ring (0.0 to 1.0)
        float ringLocalProgress = (ringProgress - ring) / 1.0f;
        ringLocalProgress = constrain(ringLocalProgress, 0.0f, 1.0f);

        // Update progress for all pixels in this ring
        m_pixelProgress[pixel1] = ringLocalProgress;
        m_pixelProgress[pixel2] = ringLocalProgress;
        m_pixelProgress[pixel3] = ringLocalProgress;
        m_pixelProgress[pixel4] = ringLocalProgress;
    }

    // Update all pixels based on their current progress
    for (uint16_t i = 0; i < EyeAnimationConstants::NUM_PIXELS_IN_RING; i++)
    {
        uint16_t pixelIndex = m_pixelOrder[i];

        // Calculate brightness (invert progress for closing phase)
        float brightness = 1.0f - m_pixelProgress[pixelIndex];

        // Get the pixel's current color and apply brightness
        uint32_t color = m_pixels->getPixelColor(pixelIndex);
        setPixelColorWithBrightness(pixelIndex, color, brightness * 255);
    }
    // set the center pixel to off when all others are off
    uint16_t centerPixelRef = (m_topPixel1 + numPairs) % EyeAnimationConstants::NUM_PIXELS_IN_RING;
    setPixelColorWithBrightness(
        EyeAnimationConstants::NUM_PIXELS_IN_RING + 1,
        m_pixels->getPixelColor(EyeAnimationConstants::NUM_PIXELS_IN_RING + 1),
        1.0f - m_pixelProgress[centerPixelRef] * 255);

    return true;
}

void EyeAnimation::rotateActiveColor()
{
    // Only change color if enough time has passed
    if (m_currentTime - m_lastColorChangeTime >= EyeAnimationConstants::COLOR_CHANGE_DELAY)
    {
        m_activeColor = (m_activeColor == EyeAnimationConstants::COLOR_BLUE)
                            ? EyeAnimationConstants::COLOR_GREEN
                            : EyeAnimationConstants::COLOR_BLUE;
        m_lastColorChangeTime = m_currentTime;
    }
}
