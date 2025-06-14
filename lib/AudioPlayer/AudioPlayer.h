/**
 * @file AudioPlayer.h
 * @brief Audio player for WAV file playback
 */

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <BackgroundAudioWAV.h>
#include <Logger.h>
#include <PWMAudio.h>
#include <WavData.h>

/**
 * @brief Represents the playback state of the audio player
 */
enum class WAVState
{
    Stopped,  ///< No audio is playing
    Playing,  ///< Audio is currently playing
    Paused    ///< Playback is paused
};

/**
 * @brief Handles playback of WAV audio files
 *
 * This class manages the playback of WAV audio files using the ROMBackgroundAudioWAV
 * library. It supports playing specific sounds by index or playing random sounds
 * from the available sound bank.
 */
class AudioPlayer
{
public:
    /**
     * @brief Construct a new Audio Player
     * @param player Pointer to the ROMBackgroundAudioWAV instance to use for playback
     */
    explicit AudioPlayer(ROMBackgroundAudioWAV* player);

    virtual ~AudioPlayer() = default;

    // Prevent copying and assignment
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;

    /**
     * @brief Play a sound by index
     * @param index Zero-based index of the sound to play
     * @return true if playback started successfully, false otherwise
     */
    virtual bool play(int index);

    /**
     * @brief Stop the currently playing sound
     */
    virtual void stop();

    /**
     * @brief Update the audio player state
     *
     * This should be called regularly from the main loop to handle
     * state transitions and cleanup.
     */
    virtual void update();

    /**
     * @brief Play a random sound from the available sounds
     * @return true if a sound was selected and playback started, false otherwise
     */
    virtual bool playRandomSound();

    /**
     * @brief Get the current playback state
     * @return Current WAVState
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
     * @return Number of sound files available
     */
    static constexpr size_t getSoundCount() { return NUM_SOUND_FILES; }

private:
    ROMBackgroundAudioWAV* m_player = nullptr;  ///< Underlying audio player
    WAVState m_state = WAVState::Stopped;       ///< Current playback state
    int m_currentSoundIndex = -1;               ///< Index of current sound, or -1 if none

    // WAV data storage
    const uint8_t* m_soundData[NUM_SOUND_FILES] = {nullptr};  ///< Pointers to sound data
    size_t m_soundDataSizes[NUM_SOUND_FILES] = {0};           ///< Sizes of sound data

    /**
     * @brief Check if a sound index is valid
     * @param index Index to check
     * @return true if the index is valid, false otherwise
     */
    bool isValidIndex(int index) const
    {
        return index >= 0 && index < static_cast<int>(NUM_SOUND_FILES);
    }
};

#endif  // AUDIO_PLAYER_H
