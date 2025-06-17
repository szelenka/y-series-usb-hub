/**
 * @file AudioPlayer.cpp
 * @brief Implementation of the AudioPlayer class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file contains the implementation of the AudioPlayer class which handles
 * WAV audio file playback using the TimerAudio library.
 */

#include "AudioPlayer.h"

/**
 * @brief Construct a new AudioPlayer instance
 *
 * @param[in] player Pointer to the TimerAudio instance
 *
 * @note The constructor initializes the audio player and sets up sound data pointers.
 *       If initialization fails, the instance will be in an error state.
 */
AudioPlayer::AudioPlayer(TimerAudio* player)
    : m_player(player), m_state(WAVState::Stopped), m_currentSoundIndex(-1)
{
    if (!m_player)
    {
        Log.error("Null player provided to AudioPlayer constructor");
        return;
    }

    Log.info("AudioPlayer initialized with TimerAudio");
}

/**
 * @brief Play a sound by its index
 *
 * @param[in] index Zero-based index of the sound to play
 * @return true if playback started successfully
 * @return false if the index is invalid or playback failed
 *
 * @note If another sound is currently playing, it will be stopped first
 */
bool AudioPlayer::play(int index)
{
    // Validate parameters and state
    if (!m_player)
    {
        Log.error("Cannot play - audio player not initialized");
        return false;
    }

    if (!isValidIndex(index))
    {
        Log.error("Invalid sound index: %d (valid: 0-%zu)", index, NUM_SOUND_FILES - 1);
        return false;
    }

    // Stop any currently playing sound
    if (m_state == WAVState::Playing)
    {
        stop();
    }

    // Start playback of the requested sound using TimerAudio
    Log.info("Starting playback of sound %d", index);
    m_player->playWAV(index);

    if (m_player->isPlaying())
    {
        m_state = WAVState::Playing;
        m_currentSoundIndex = index;
        Log.info("Sound %d playback started successfully", index);
        return true;
    }
    else
    {
        Log.error("Failed to start playback of sound %d", index);
        return false;
    }
}

/**
 * @brief Stop the currently playing sound
 *
 * If no sound is playing, this method does nothing.
 * The player state will be set to WAVState::Stopped.
 */
void AudioPlayer::stop()
{
    if (!m_player)
    {
        Log.error("Cannot stop - audio player not initialized");
        return;
    }

    if (m_state == WAVState::Playing)
    {
        Log.info("Stopping playback of sound %d", m_currentSoundIndex);
        m_player->stop();
    }

    m_state = WAVState::Stopped;
    m_currentSoundIndex = -1;
}

/**
 * @brief Update the audio player state
 *
 * This method should be called regularly from the main program loop
 * to handle state transitions and cleanup. It checks if the current
 * playback has completed and updates the internal state accordingly.
 */
void AudioPlayer::update()
{
    if (!m_player)
    {
        return;
    }

    // Check if playback has completed
    if (m_state == WAVState::Playing && !m_player->isPlaying())
    {
        Log.info("Sound %d playback completed", m_currentSoundIndex);
        m_state = WAVState::Stopped;
        m_currentSoundIndex = -1;
    }
}

/**
 * @brief Play a random sound from the available sounds
 *
 * @return true if a sound was selected and playback started
 * @return false if no sounds are available or playback failed
 *
 * @note This will never play the sound at index 0 to avoid system sounds
 */
bool AudioPlayer::playRandomSound()
{
    if (NUM_SOUND_FILES <= 1)
    {
        Log.warning("No sounds available for random playback");
        return false;
    }

    // Generate random index (skip index 0 for system sounds)
    const int randomIndex = (rand() % (NUM_SOUND_FILES - 1)) + 1;

    Log.info("Playing random sound %d", randomIndex);
    return play(randomIndex);
}
