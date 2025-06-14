/**
 * @file AudioPlayer.cpp
 * @brief Implementation of the AudioPlayer class
 */
#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(ROMBackgroundAudioWAV* player) : m_player(player)
{
    // Initialize the audio player
    if (!m_player->begin())
    {
        Log.error("Failed to initialize audio player");
        return;
    }

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
    if (!isValidIndex(index))
    {
        Log.error("Invalid sound index: %d", index);
        return false;
    }

    // Ensure any currently playing sound is stopped first
    if (m_state == WAVState::Playing)
    {
        stop();
    }

    // Start playback
    Log.info("Start playback: %d", index);
    Log.debug("Sound data pointer: %p, size: %zu", (void*)m_soundData[index], m_soundDataSizes[index]);
    const size_t writeResult = m_player->write(m_soundData[index], m_soundDataSizes[index]);
    Log.debug("Sound data write result: %zu", writeResult);
    if (writeResult > 0)
    {
        m_state = WAVState::Playing;
        m_currentSoundIndex = index;
        return true;
    }
    Log.error("Failed to start playback");

    return false;
}

void AudioPlayer::stop()
{
    m_player->flush();
    Log.info("Stop playback: %d", m_currentSoundIndex);
    m_state = WAVState::Stopped;
    m_currentSoundIndex = -1;
}

void AudioPlayer::update()
{
    // Automatically transition from Playing to Stopped when playback completes
    if (m_state == WAVState::Playing && m_player->done())
    {
        stop();
    }
}

bool AudioPlayer::playRandomSound()
{
    int index = random(1, max(1, static_cast<int>(getSoundCount())));
    return play(index);
}
