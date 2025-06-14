/**
 * @file EyeAnimation.h
 * @brief Handles all eye-related animations for the Y-Series USB Hub
 */

#pragma once

#include <Adafruit_NeoPixel.h>
#include <Logger.h>

class EyeAnimation
{
public:
    /**
     * @brief Construct a new Eye Animation controller
     * @param pixels Pointer to the NeoPixel controller
     * @param numPixels Number of pixels in the eye ring
     */
    EyeAnimation(Adafruit_NeoPixel* pixels);

    /**
     * @brief Set the top pixels for the blink animation
     * @param topPixel1 First top pixel (0-15)
     * @param topPixel2 Second top pixel (0-15)
     */
    virtual void setTopPixels(uint8_t topPixel1, uint8_t topPixel2);

    /**
     * @brief Set the default color for the eyes
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    virtual void setDefaultColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Set the global brightness
     * @param brightness Brightness value (0-255)
     */
    virtual void setBrightness(uint8_t brightness);

    /**
     * @brief Set the current time
     * @param currentTime Current time in milliseconds
     */
    virtual void setCurrentTime(unsigned long currentTime) { m_currentTime = currentTime; }

    /**
     * @brief Update the eye animation
     */
    virtual void updateRainbow();
    /**
     * @brief Update the eye animation
     */
    virtual void updateDefault();

    /**
     * @brief Start a blink animation
     * @param duration Total duration of the blink in milliseconds
     */
    virtual void blink(unsigned long duration = 200);

    /**
     * @brief Update the blink animation state
     * @return true if blink is in progress, false otherwise
     */
    virtual bool updateBlink();

    /**
     * @brief Start a sequence of blinks
     */
    virtual void sequenceBlink();

private:
    virtual void setAllPixelsColor(uint32_t color);
    virtual void setPixelColorWithBrightness(uint16_t pixel, uint32_t color,
                                             uint8_t brightness = 255);
    virtual uint32_t wheel(uint8_t pos);

    Adafruit_NeoPixel* m_pixels = nullptr;

    // Animation state
    uint16_t m_rainbowIndex;
    unsigned long m_rainbowTimer;
    uint32_t m_defaultColor;
    uint8_t m_brightness;
    unsigned long m_currentTime;

    /**
     * @brief Calculate the order in which pixels should animate during a blink
     * This is called automatically when top pixels are set
     */
    virtual void calculatePixelOrder();

    // Blink state
    bool m_isBlinking;
    unsigned long m_blinkStartTime;
    unsigned long m_blinkDuration;
    unsigned long m_blinkEndTime;
    uint8_t m_blinkPhase;                // 0=not blinking, 1=closing, 2=opening
    float m_blinkProgress;               // 0.0 to 1.0 for current phase
    float m_pixelProgress[16];           // Progress for each pixel (0.0 to 1.0)
    uint8_t m_topPixel1;                 // First top pixel
    uint8_t m_topPixel2;                 // Second top pixel
    uint8_t m_pixelOrder[16];            // Calculated pixel order for animation
    unsigned long m_nextBlinkDelay = 0;  ///< Delay until next blink
    uint8_t m_blinkCount = 0;            ///< Number of blinks
};
