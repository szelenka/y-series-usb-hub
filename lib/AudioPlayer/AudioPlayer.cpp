/**
 * @file AudioPlayer.cpp
 * @brief Implementation of the AudioPlayer class for Y-Series USB Hub
 * @author Scott Zelenka
 * @date 2024-06-14
 *
 * @details
 * This file contains the implementation of the AudioPlayer class which handles
 * WAV audio file playback using the ROMBackgroundAudioWAV library.
 */

#include "AudioPlayer.h"

/**
 * @brief Construct a new AudioPlayer instance
 *
 * @param[in] player Pointer to the ROMBackgroundAudioWAV instance
 *
 * @note The constructor initializes the audio player and sets up sound data pointers.
 *       If initialization fails, the instance will be in an error state.
 */
AudioPlayer::AudioPlayer(ROMBackgroundAudioWAV* player)
    : m_player(player), m_state(WAVState::Stopped), m_currentSoundIndex(-1)
{
    if (!m_player)
    {
        Log.error("Null player provided to AudioPlayer constructor");
        return;
    }

    // Initialize the audio player hardware
    if (!m_player->begin())
    {
        Log.error("Failed to initialize audio player");
        return;
    }

    // Initialize WAV data pointers from program memory
    for (size_t i = 0; i < NUM_SOUND_FILES; i++)
    {
        m_soundData[i] = sound_data_pointers[i];
        m_soundDataSizes[i] = sound_data_sizes[i];
        Log.debug("Sound %zu: %p, %zu bytes", i, m_soundData[i], m_soundDataSizes[i]);
    }
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

    // Start playback of the requested sound
    Log.info("Starting playback of sound %d", index);
    const size_t writeResult = m_player->write(m_soundData[index], m_soundDataSizes[index]);

    if (writeResult > 0)
    {
        m_state = WAVState::Playing;
        m_currentSoundIndex = index;
        Log.debug("Playback started: %d (%zu bytes written)", index, writeResult);
        return true;
    }

    Log.error("Failed to start playback of sound %d (write result: %zu)", index, writeResult);
    return false;
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
        return;  // Nothing to stop if player is not initialized
    }

    Log.debug("Stopping playback of sound %d", m_currentSoundIndex);
    m_player->flush();
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
    // Automatically transition from Playing to Stopped when playback completes
    if (m_state == WAVState::Playing && m_player->done())
    {
        Log.debug("Playback completed for sound %d", m_currentSoundIndex);
        stop();
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
        Log.warning("Not enough sounds available for random selection");
        return false;
    }

    // Generate random index between 0 and NUM_SOUND_FILES-1
    const int index = random(NUM_SOUND_FILES - 1);
    Log.debug("Selected random sound: %d/%zu", index, NUM_SOUND_FILES - 1);

    return play(index);
}
