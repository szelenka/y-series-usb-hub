/**
 * @file TimerAudio.h
 * @brief Timer-based PWM audio player for WAV data from headers
 * @author Scott Zelenka
 * @date 2024-06-17
 *
 * @details
 * This module provides timer-based PWM audio playback for WAV audio files
 * stored in program memory (PROGMEM). It's designed for use with the
 * Y-Series USB Hub audio system and supports differential PWM output
 * for high-quality audio reproduction through Class D amplifiers.
 *
 * Features:
 * - Timer-interrupt driven audio playback at configurable sample rates
 * - Differential PWM output for superior audio quality
 * - Automatic WAV header parsing and data extraction
 * - Direct integration with WavData module for PROGMEM audio storage
 * - Designed specifically for use with PAM8302 Class D amplifiers
 */

#ifndef Y_SERIES_USB_HUB_TIMER_AUDIO_H
#define Y_SERIES_USB_HUB_TIMER_AUDIO_H

// System includes
#include <Arduino.h>
#ifdef ARDUINO_ARCH_RP2040
#include <hardware/pwm.h>
#include <hardware/irq.h>
#endif

// Project includes
#include <WavData.h>

/**
 * @brief Contains constants used by the TimerAudio class
 */
namespace TimerAudioConstants
{
/// @name Audio Configuration
/// @{
constexpr uint32_t PWM_RESOLUTION = 255;         ///< PWM resolution for 8-bit audio
constexpr uint32_t DEFAULT_SAMPLE_RATE = 22050;  ///< Default sample rate in Hz
constexpr size_t WAV_HEADER_SIZE = 44;           ///< Standard WAV header size in bytes
constexpr uint8_t SILENCE_LEVEL = 128;           ///< PWM value for audio silence (8-bit center)
/// @}
}  // namespace TimerAudioConstants

/**
 * @brief Timer-based PWM audio player for WAV file playback
 *
 * @details
 * The TimerAudio class provides hardware-accelerated audio playback using
 * timer interrupts and PWM output. It reads WAV data from program memory
 * and converts it to differential PWM signals suitable for Class D amplifiers.
 *
 * The class handles all the low-level details of PWM configuration, timer
 * setup, and audio data processing while providing a simple interface for
 * audio playback control.
 */
class TimerAudio
{
public:
    /// @name Construction and Initialization
    /// @{
    /**
     * @brief Construct a new TimerAudio player
     *
     * @param[in] pinPos GPIO pin for positive PWM output (A+)
     * @param[in] pinNeg GPIO pin for negative PWM output (A-)
     * @param[in] sampleRate Sample rate in Hz (default: 22050)
     *
     * @note Both pins must be PWM-capable on the RP2040
     * @warning The provided pins must remain valid for the lifetime of this object
     */
    TimerAudio(uint8_t pinPos, uint8_t pinNeg,
               uint32_t sampleRate = TimerAudioConstants::DEFAULT_SAMPLE_RATE);

    /**
     * @brief Destructor - cleans up timer and PWM resources
     */
    ~TimerAudio();

    // Prevent copying and assignment
    TimerAudio(const TimerAudio&) = delete;
    TimerAudio& operator=(const TimerAudio&) = delete;
    /// @}

    /// @name System Control
    /// @{
    /**
     * @brief Initialize the audio system
     *
     * @note This must be called before any audio playback
     */
    void begin();
    /// @}

    /// @name Playback Control
    /// @{
    /**
     * @brief Play a WAV file by index
     *
     * @param[in] wavIndex Index of the WAV file to play (0 to NUM_SOUND_FILES-1)
     *
     * @note If another sound is playing, it will be stopped first
     * @see WavData.h for available sound indices
     */
    void playWAV(uint8_t wavIndex);

    /**
     * @brief Stop the currently playing audio
     *
     * @note Sets PWM outputs to silence level
     */
    void stop();

    /**
     * @brief Check if audio is currently playing
     *
     * @return true if audio is playing, false otherwise
     */
    bool isPlaying() const { return m_isPlaying; }
    /// @}

    /// @name Internal Methods (called by timer interrupt)
    /// @{
    /**
     * @brief Update audio sample output
     *
     * @note This is called automatically by the timer interrupt
     * @warning Do not call this method directly
     */
    void updateSample();
    /// @}

private:
    /// @name Hardware Configuration
    /// @{
    uint8_t m_pinAudioPos;  ///< Positive PWM output pin (A+)
    uint8_t m_pinAudioNeg;  ///< Negative PWM output pin (A-)
#ifdef ARDUINO_ARCH_RP2040
    uint m_pwmSlicePos;  ///< PWM slice for positive output
    uint m_pwmSliceNeg;  ///< PWM slice for negative output
#endif
    uint32_t m_sampleRate;      ///< Sample rate in Hz
    repeating_timer_t m_timer;  ///< Hardware timer for sample timing
    /// @}

    /// @name Audio State
    /// @{
    volatile const uint8_t* m_currentWavData;  ///< Pointer to current WAV data
    volatile size_t m_currentWavSize;          ///< Size of current WAV data
    volatile size_t m_currentPosition;         ///< Current playback position
    volatile bool m_isPlaying;                 ///< True if audio is playing
    volatile bool m_skipWavHeader;             ///< True to skip WAV headers
    /// @}

    /// @name Internal Methods
    /// @{
    /**
     * @brief Configure PWM hardware for audio output
     */
    void setupPWM();

    /**
     * @brief Configure timer for sample rate timing
     */
    void setupTimer();
    /// @}

    /// @name Static Members
    /// @{
    static TimerAudio* s_instance;  ///< Static instance for timer callback
    /// @}
};

#endif  // Y_SERIES_USB_HUB_TIMER_AUDIO_H
