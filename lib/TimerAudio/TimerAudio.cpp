/**
 * @file TimerAudio.cpp
 * @brief Implementation of the TimerAudio class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-17
 *
 * @details
 * This file contains the implementation of the TimerAudio class which handles
 * timer-based PWM audio playback using hardware interrupts and differential
 * PWM output for high-quality audio reproduction.
 */

#include "TimerAudio.h"
#ifdef ARDUINO_ARCH_RP2040
#include <hardware/timer.h>
#include <hardware/clocks.h>
#endif

// Static instance for timer callback
TimerAudio* TimerAudio::s_instance = nullptr;

/**
 * @brief Construct a new TimerAudio player
 *
 * @param[in] pinPos GPIO pin for positive PWM output (A+)
 * @param[in] pinNeg GPIO pin for negative PWM output (A-)
 * @param[in] sampleRate Sample rate in Hz
 */
TimerAudio::TimerAudio(uint8_t pinPos, uint8_t pinNeg, uint32_t sampleRate)
    : m_pinAudioPos(pinPos),
      m_pinAudioNeg(pinNeg),
      m_sampleRate(sampleRate),
      m_currentWavData(nullptr),
      m_currentWavSize(0),
      m_currentPosition(0),
      m_isPlaying(false),
      m_skipWavHeader(true),
      m_timer()
{
    s_instance = this;
}

/**
 * @brief Destructor - cleans up timer and PWM resources
 */
TimerAudio::~TimerAudio()
{
    stop();
    s_instance = nullptr;
}

/**
 * @brief Initialize the audio system
 *
 * @note This must be called before any audio playback
 */
void TimerAudio::begin()
{
#ifdef ARDUINO_ARCH_RP2040
    setupPWM();
    setupTimer();
#endif
}

/**
 * @brief Configure PWM hardware for audio output
 */
void TimerAudio::setupPWM()
{
#ifdef ARDUINO_ARCH_RP2040
    // Get PWM slices for both pins
    m_pwmSlicePos = pwm_gpio_to_slice_num(m_pinAudioPos);
    m_pwmSliceNeg = pwm_gpio_to_slice_num(m_pinAudioNeg);

    // Set pins to PWM function
    gpio_set_function(m_pinAudioPos, GPIO_FUNC_PWM);
    gpio_set_function(m_pinAudioNeg, GPIO_FUNC_PWM);

    // Configure PWM for audio output
    // Set PWM frequency high enough to avoid audible switching noise
    pwm_config config = pwm_get_default_config();

    // Set wrap to 255 for 8-bit resolution (matches our WAV data)
    pwm_config_set_wrap(&config, TimerAudioConstants::PWM_RESOLUTION);

    // Apply configuration to both slices
    pwm_init(m_pwmSlicePos, &config, true);
    pwm_init(m_pwmSliceNeg, &config, true);

    // Set initial duty cycles to center (silence)
    pwm_set_gpio_level(m_pinAudioPos, TimerAudioConstants::SILENCE_LEVEL);
    pwm_set_gpio_level(m_pinAudioNeg, TimerAudioConstants::SILENCE_LEVEL);
#endif
}

/**
 * @brief Configure timer for sample rate timing
 */
void TimerAudio::setupTimer()
{
#ifdef ARDUINO_ARCH_RP2040
    // Calculate timer interval for sample rate
    // Timer runs at 1MHz, so interval = 1,000,000 / sample_rate
    uint32_t timerInterval = 1000000 / m_sampleRate;

    // Add repeating timer
    add_repeating_timer_us(
        -timerInterval,
        [](repeating_timer_t* rt) -> bool
        {
            if (TimerAudio::s_instance)
            {
                TimerAudio::s_instance->updateSample();
            }
            return true;  // Continue repeating
        },
        NULL, &m_timer);
#endif
}

/**
 * @brief Play a WAV file by index
 *
 * @param[in] wavIndex Index of the WAV file to play (0 to NUM_SOUND_FILES-1)
 *
 * @note If another sound is playing, it will be stopped first
 */
void TimerAudio::playWAV(uint8_t wavIndex)
{
    // Stop current playback
    stop();

    // Get WAV data from WavData module
    m_currentWavData = getWavData(wavIndex);
    m_currentWavSize = getWavSize(wavIndex);

    if (m_currentWavData && m_currentWavSize > 0)
    {
        // Skip WAV header if present
        if (m_skipWavHeader && m_currentWavSize > TimerAudioConstants::WAV_HEADER_SIZE)
        {
            // Look for "data" chunk in first 100 bytes
            for (size_t i = 0; i < min(100ul, m_currentWavSize - 4); i++)
            {
                if (pgm_read_byte((const void*)&m_currentWavData[i]) == 'd' &&
                    pgm_read_byte((const void*)&m_currentWavData[i + 1]) == 'a' &&
                    pgm_read_byte((const void*)&m_currentWavData[i + 2]) == 't' &&
                    pgm_read_byte((const void*)&m_currentWavData[i + 3]) == 'a')
                {
                    m_currentPosition = i + 8;  // Skip "data" + size bytes
                    break;
                }
            }
            if (m_currentPosition == 0)
            {
                m_currentPosition =
                    TimerAudioConstants::WAV_HEADER_SIZE;  // Fallback to standard header size
            }
        }
        else
        {
            m_currentPosition = 0;
        }

        m_isPlaying = true;
    }
}

/**
 * @brief Stop the currently playing audio
 *
 * @note Sets PWM outputs to silence level
 */
void TimerAudio::stop()
{
    m_isPlaying = false;
    m_currentPosition = 0;

#ifdef ARDUINO_ARCH_RP2040
    // Set outputs to silence (center)
    pwm_set_gpio_level(m_pinAudioPos, TimerAudioConstants::SILENCE_LEVEL);
    pwm_set_gpio_level(m_pinAudioNeg, TimerAudioConstants::SILENCE_LEVEL);
#endif
}

/**
 * @brief Update audio sample output
 *
 * @note This is called automatically by the timer interrupt
 * @warning Do not call this method directly
 */
void TimerAudio::updateSample()
{
    if (!m_isPlaying || !m_currentWavData || m_currentPosition >= m_currentWavSize)
    {
        if (m_isPlaying)
        {
            stop();  // Auto-stop when finished
        }
        return;
    }

    // Read next audio sample from PROGMEM
    uint8_t sample = pgm_read_byte((const void*)&m_currentWavData[m_currentPosition]);
    m_currentPosition++;

#ifdef ARDUINO_ARCH_RP2040
    // Convert 8-bit WAV sample to differential PWM
    // WAV data is 0x80 centered (128), so we use it directly

    // For better audio quality, use differential signaling:
    // A+ gets the sample value
    // A- gets the inverted sample value
    pwm_set_gpio_level(m_pinAudioPos, sample);
    pwm_set_gpio_level(m_pinAudioNeg, 255 - sample);
#endif
}
