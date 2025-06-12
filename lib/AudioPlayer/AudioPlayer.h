#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <BackgroundAudioWAV.h>
#include <PWMAudio.h>
#include <WavData.h>
// WAV player state
enum class WAVState {
    Stopped,
    Playing,
    Paused
};

class AudioPlayer {
public:
    AudioPlayer(ROMBackgroundAudioWAV* player) :
        m_player(player),
        m_state(WAVState::Stopped),
        m_currentSoundIndex(-1) 
    {
        std::cout << "AudioPlayer constructor called" << std::endl;
        if (m_player != nullptr) {
            m_player->begin();
            // Initialize WAV data pointers
            for (int i = 0; i < NUM_SOUND_FILES; i++) {
                m_soundData[i] = sound_data_pointers[i];
                m_soundDataSizes[i] = sound_data_sizes[i];
            }
        }
    }
    
    virtual void play(int index);
    virtual void stop();
    virtual void update();
    virtual void playRandomSound();
    
    virtual WAVState getState() const { return m_state; }
    virtual int getCurrentSoundIndex() const { return m_currentSoundIndex; }
    virtual bool isPlaying() const { return m_state == WAVState::Playing; }
    
private:
    ROMBackgroundAudioWAV* m_player = nullptr;
    WAVState m_state;
    int m_currentSoundIndex;
    
    // WAV data pointers
    const uint8_t* m_soundData[NUM_SOUND_FILES];
    size_t m_soundDataSizes[NUM_SOUND_FILES];
};

#endif // AUDIO_PLAYER_H
