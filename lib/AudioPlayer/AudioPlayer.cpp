#include "AudioPlayer.h"

void AudioPlayer::play(int index) {
    if (index < 0 || index >= NUM_SOUND_FILES) {
        return;
    }
    
    m_state = WAVState::Playing;
    m_currentSoundIndex = index;
    m_player->write(m_soundData[index], m_soundDataSizes[index]);
}

void AudioPlayer::stop() {
    m_state = WAVState::Stopped;
    m_currentSoundIndex = -1;
    m_player->flush();
}

void AudioPlayer::update() {
    if (m_state == WAVState::Playing && !m_player->playing()) {
        stop();
    }
}

void AudioPlayer::playRandomSound() {
    int index;
#ifdef ARDUINO
    index = random(0, NUM_SOUND_FILES);
#else
    int max = RAND_MAX / NUM_SOUND_FILES * NUM_SOUND_FILES;  // Get max value that's divisible by NUM_SOUND_FILES
    do {
        index = rand();  // Get random number
    } while (index >= max);  // Reject numbers that would cause bias
    index = index % NUM_SOUND_FILES;  // Now safe to modulo
#endif
    play(index);
}
