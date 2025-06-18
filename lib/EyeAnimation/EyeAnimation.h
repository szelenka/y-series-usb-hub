/**
 * @file EyeAnimation.h
 * @brief Eye animation controller for the Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file defines the EyeAnimation class which manages LED animations for the
 * Y-Series USB Hub's eye display. It supports various eye effects including
 * blinking, color changes, and rainbow animations using NeoPixel LEDs.
 *
 * The EyeAnimation is responsible for:
 * - Managing NeoPixel LED states and colors
 * - Handling smooth eye blinking animations
 * - Supporting different eye display modes (solid color, rainbow, etc.)
 * - Providing a clean interface for eye animation control
 */

#ifndef Y_SERIES_USB_HUB_EYE_ANIMATION_H
#define Y_SERIES_USB_HUB_EYE_ANIMATION_H

// System includes
#include <Arduino.h>

// Third-party includes
#include <Adafruit_NeoPixel.h>

// Project-local includes
#include <Logger.h>

/**
 * @brief Contains constants used by the EyeAnimation class
 */
namespace EyeAnimationConstants
{
/// @name Color Definitions
/// @{
constexpr uint32_t COLOR_BLACK = 0x000000;  ///< Black (off)
constexpr uint32_t COLOR_BLUE = 0x21DDF5;   ///< Blue eye
constexpr uint32_t COLOR_GREEN = 0x0BBD39;  ///< Green eye
/// @}

constexpr uint16_t NUM_PIXELS_IN_RING = 16;                    // Number of LEDs in the eye ring
constexpr uint8_t DEFAULT_BRIGHTNESS = 255;            // Maximum brightness
constexpr unsigned long DEFAULT_BLINK_DURATION = 300;  // ms for a complete blink
constexpr unsigned long COLOR_CHANGE_DELAY = 1000;     // ms between color changes
};  // namespace EyeAnimationConstants

/**
 * @brief Controls eye animations using NeoPixel LEDs
 *
 * @details
 * The EyeAnimation class provides a high-level interface for creating smooth,
 * visually appealing eye animations. It handles all the low-level details of
 * NeoPixel control while providing simple methods for common eye animations.
 */
class EyeAnimation
{
public:
    /// @name Construction and Initialization
    /// @{

    /**
     * @brief Construct a new Eye Animation controller
     *
     * @param[in] pixels Pointer to the Adafruit_NeoPixel instance
     *
     * @note The NeoPixel instance must be initialized before use
     * @warning The NeoPixel pointer must remain valid for the lifetime of this object
     */
    explicit EyeAnimation(Adafruit_NeoPixel* pixels);

    // Prevent copying and assignment
    EyeAnimation(const EyeAnimation&) = delete;
    EyeAnimation& operator=(const EyeAnimation&) = delete;

    /// @}

    /// @name Configuration
    /// @{

    /**
     * @brief Set the top pixels for blink animation
     *
     * @param[in] topPixel1 First top pixel index (0-15)
     * @param[in] topPixel2 Second top pixel index (0-15)
     *
     * @note This affects the direction of the blink animation
     */
    virtual void setTopPixels(uint8_t topPixel1, uint8_t topPixel2);

    /**
     * @brief Set the active eye color
     *
     * @param[in] color 32-bit color value (0x00RRGGBB)
     */
    virtual void setActiveColor(uint32_t color) { m_activeColor = color; }

    /**
     * @brief Set the global brightness
     *
     * @param[in] brightness Brightness value (0-255)
     */
    virtual void setBrightness(uint8_t brightness) { m_brightness = brightness; }

    /**
     * @brief Set the current time for animation timing
     *
     * @param[in] currentTime Current time in milliseconds
     */
    virtual void setCurrentTime(unsigned long currentTime) { m_currentTime = currentTime; }

    /// @}

    /// @name Animation Control
    /// @{

    /**
     * @brief Update the eyes with a rainbow animation effect
     *
     * @note This should be called regularly from the main loop
     */
    virtual void updateRainbowColor();

    /**
     * @brief Update the eyes with the active solid color
     *
     * @note This should be called regularly from the main loop
     */
    virtual void updateActiveColor();

    /**
     * @brief Rotate the eye color in sequence
     */
    virtual void rotateActiveColor();

    /**
     * @brief Put the eye animation to sleep
     */
    virtual void sleep();

    /**
     * @brief Start a blink animation
     *
     * @param[in] duration Total duration of the blink in milliseconds
     */
    virtual void blink(unsigned long duration = 200);

    /**
     * @brief Update the blink animation state
     *
     * @return true if a blink is in progress, false otherwise
     *
     * @note This should be called regularly from the main loop
     */
    virtual bool updateBlink();

    /**
     * @brief Start a sequence of blinks (blink multiple times)
     *
     * @note The number and timing of blinks is controlled by internal constants
     */
    virtual void sequenceBlink();

    /// @}

protected:
    /// @name Internal Methods
    /// @{

    /**
     * @brief Set all pixels to the specified color
     *
     * @param[in] color 32-bit color value (0x00RRGGBB)
     */
    virtual void setAllPixelsColor(uint32_t color);

    /**
     * @brief Set a single pixel's color with brightness adjustment
     *
     * @param[in] pixel Pixel index
     * @param[in] color 32-bit color value (0x00RRGGBB)
     * @param[in] brightness Brightness value (0-255)
     */
    virtual void setPixelColorWithBrightness(uint16_t pixel, uint32_t color,
                                             uint8_t brightness = 255);

    /**
     * @brief Generate a color from a position on the color wheel
     *
     * @param[in] pos Position on the color wheel (0-255)
     * @return uint32_t Color value (0x00RRGGBB)
     */
    virtual uint32_t wheel(uint8_t pos);

    /**
     * @brief Calculate the order in which pixels should animate during a blink
     *
     * @note This is called automatically when top pixels are set
     */
    virtual void calculatePixelOrder();

    /// @}

private:
    /// @name Member Variables
    /// @{

    Adafruit_NeoPixel* m_pixels;  ///< Pointer to NeoPixel controller

    // Animation state
    uint16_t m_rainbowIndex;       ///< Current position in rainbow animation
    unsigned long m_rainbowTimer;  ///< Timer for rainbow animation updates
    uint32_t m_activeColor;        ///< Active eye color (0x00RRGGBB)
    uint8_t m_brightness;          ///< Global brightness (0-255)
    unsigned long m_currentTime;   ///< Current time in milliseconds

    // Blink state
    bool m_isBlinking;                    ///< True if a blink is in progress
    unsigned long m_blinkStartTime;       ///< When the current blink started
    unsigned long m_blinkDuration;        ///< Duration of the current blink
    unsigned long m_blinkEndTime;         ///< When the current blink sequence ends
    uint8_t m_blinkPhase;                 ///< 0=not blinking, 1=closing, 2=opening
    float m_blinkProgress;                ///< 0.0 to 1.0 for current blink phase
    float m_pixelProgress[16];            ///< Progress for each pixel (0.0 to 1.0)
    uint8_t m_topPixel1;                  ///< First top pixel index
    uint8_t m_topPixel2;                  ///< Second top pixel index
    uint8_t m_pixelOrder[16];             ///< Animation order for pixels during blink
    unsigned long m_nextBlinkDelay;       ///< Delay until next blink in sequence
    uint8_t m_blinkCount;                 ///< Number of blinks in current sequence
    unsigned long m_lastColorChangeTime;  ///< Time of last color change

    /// @}
};

#endif  // Y_SERIES_USB_HUB_EYE_ANIMATION_H
