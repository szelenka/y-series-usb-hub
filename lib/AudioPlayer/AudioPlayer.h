/**
 * @file AudioPlayer.h
 * @brief Audio player for WAV file playback in the Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file defines the AudioPlayer class which manages WAV audio file playback
 * using the ROMBackgroundAudioWAV library. It provides an interface for playing
 * sounds by index or randomly from a predefined set of audio files.
 *
 * The AudioPlayer is responsible for:
 * - Managing audio playback state
 * - Handling sound data storage and retrieval
 * - Providing playback control (play, stop)
 * - Supporting random sound selection
 */

#ifndef Y_SERIES_USB_HUB_AUDIO_PLAYER_H
#define Y_SERIES_USB_HUB_AUDIO_PLAYER_H

// System includes
#include <Arduino.h>

// Project includes
#include <BackgroundAudioWAV.h>
#include <PWMAudio.h>

// Project-local includes
#include <Logger.h>
#include <WavData.h>

/**
 * @brief Enumerates possible states of audio playback
 *
 * This enum class represents the different states that the audio player
 * can be in during its lifecycle.
 */
enum class WAVState
{
    Stopped = 0,  ///< No audio is currently playing
    Playing = 1,  ///< Audio is currently playing
    Paused = 2    ///< Playback is paused (currently unused)
};

/**
 * @brief Main controller for audio playback functionality
 *
 * @details
 * The AudioPlayer class manages the playback of WAV audio files using the
 * ROMBackgroundAudioWAV library. It provides a simple interface for playing
 * sounds by index or randomly selecting from the available sound bank.
 *
 * The class handles all audio playback state management and provides
 * methods for controlling playback and querying the current state.
 */
class AudioPlayer
{
public:
    /// @name Construction and Destruction
    /// @{

    /**
     * @brief Construct a new AudioPlayer instance
     *
     * @param[in] player Pointer to the ROMBackgroundAudioWAV instance to use for playback
     * @note The provided player instance must remain valid for the lifetime of this AudioPlayer
     * @warning Passing a null player will result in a non-functional AudioPlayer instance
     */
    explicit AudioPlayer(ROMBackgroundAudioWAV* player);

    /**
     * @brief Virtual destructor for proper cleanup in derived classes
     */
    virtual ~AudioPlayer() = default;

    // Prevent copying and assignment
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    /// @}

    /// @name Playback Control
    /// @{

    /**
     * @brief Play a sound by its index
     *
     * @param[in] index Zero-based index of the sound to play
     * @return true if playback started successfully
     * @return false if the index is invalid or playback failed
     *
     * @note If another sound is currently playing, it will be stopped first
     * @see getSoundCount()
     */
    virtual bool play(int index);

    /**
     * @brief Stop the currently playing sound
     *
     * If no sound is playing, this method does nothing.
     * The player state will be set to WAVState::Stopped.
     */
    virtual void stop();

    /**
     * @brief Play a random sound from the available sounds
     *
     * @return true if a sound was selected and playback started
     * @return false if no sounds are available or playback failed
     *
     * @note This will never play the sound at index 0 to avoid system sounds
     */
    virtual bool playRandomSound();

    /// @}

    /// @name State Queries
    /// @{

    /**
     * @brief Get the current playback state
     * @return Current WAVState value
     */
    virtual WAVState getState() const { return m_state; }

    /**
     * @brief Get the index of the currently playing sound
     * @return Index of the current sound, or -1 if no sound is playing
     */
    virtual int getCurrentSoundIndex() const { return m_currentSoundIndex; }

    /**
     * @brief Check if audio is currently playing
     * @return true if audio is playing, false otherwise
     */
    virtual bool isPlaying() const { return m_state == WAVState::Playing; }

    /**
     * @brief Get the total number of available sounds
     * @return Number of sound files available (compile-time constant)
     */
    static constexpr size_t getSoundCount() { return NUM_SOUND_FILES; }

    /// @}

    /// @name System Interface
    /// @{

    /**
     * @brief Update the audio player state
     *
     * This method should be called regularly from the main program loop
     * to handle state transitions and cleanup. It checks if the current
     * playback has completed and updates the internal state accordingly.
     */
    virtual void update();

    /// @}

protected:
    /// @name Helper Methods
    /// @{

    /**
     * @brief Check if a sound index is valid
     * @param[in] index Index to validate
     * @return true if the index is within valid range, false otherwise
     */
    bool isValidIndex(int index) const
    {
        return index >= 0 && index < static_cast<int>(NUM_SOUND_FILES);
    }

    /// @}

private:
    /// @name Hardware Interface
    /// @{
    ROMBackgroundAudioWAV* m_player;  ///< Underlying audio player instance
    /// @}

    /// @name Playback State
    /// @{
    WAVState m_state = WAVState::Stopped;  ///< Current playback state
    int m_currentSoundIndex = -1;          ///< Index of current sound, or -1 if none
    /// @}

    /// @name Sound Data
    /// @{
    const uint8_t* m_soundData[NUM_SOUND_FILES] = {
        nullptr};                                    ///< Pointers to sound data in program memory
    size_t m_soundDataSizes[NUM_SOUND_FILES] = {0};  ///< Sizes of sound data in bytes
    /// @}
};

#endif  // Y_SERIES_USB_HUB_AUDIO_PLAYER_H
