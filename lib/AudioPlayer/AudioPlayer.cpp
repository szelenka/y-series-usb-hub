/**
 * @file AudioPlayer.cpp
 * @brief Implementation of the AudioPlayer class
 */

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(ROMBackgroundAudioWAV* player) : m_player(player)
{
    if (m_player == nullptr)
    {
        return;
    }

    // Initialize the audio player
    m_player->begin();

    // Initialize WAV data pointers
    for (size_t i = 0; i < NUM_SOUND_FILES; i++)
    {
        m_soundData[i] = sound_data_pointers[i];
        m_soundDataSizes[i] = sound_data_sizes[i];
    }
}

bool AudioPlayer::play(int index)
{
    // Validate index and player state
    if (!isValidIndex(index) || m_player == nullptr)
    {
        return false;
    }

    // Ensure any currently playing sound is stopped first
    if (m_state == WAVState::Playing)
    {
        stop();
    }

    // Start playback
    const size_t writeResult = m_player->write(m_soundData[index], m_soundDataSizes[index]);

    if (writeResult > 0)
    {
        m_state = WAVState::Playing;
        m_currentSoundIndex = index;
        return true;
    }

    return false;
}

void AudioPlayer::stop()
{
    if (m_player == nullptr)
    {
        return;
    }

    m_player->flush();
    m_state = WAVState::Stopped;
    m_currentSoundIndex = -1;
}

void AudioPlayer::update()
{
    if (m_player == nullptr)
    {
        return;
    }

    // Automatically transition from Playing to Stopped when playback completes
    if (m_state == WAVState::Playing && !m_player->playing())
    {
        stop();
    }
}

bool AudioPlayer::playRandomSound()
{
    if (getSoundCount() == 0)
    {
        return false;  // No sounds available
    }

    int index = random(0, static_cast<int>(getSoundCount()));
    return play(index);
}
