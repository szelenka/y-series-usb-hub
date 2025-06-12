#ifndef MOCK_BACKGROUND_AUDIO_WAV_H
#define MOCK_BACKGROUND_AUDIO_WAV_H

#include "AudioOutputBase.h"

template<typename T>
class BackgroundAudioWAVClass {
public:
    BackgroundAudioWAVClass() = default;
    BackgroundAudioWAVClass(AudioOutputBase& audio) : m_audio(audio) {}
    
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool playing() = 0;
    virtual void write(const uint8_t* data, size_t size) = 0;
    virtual void write(uint8_t c) = 0;
    virtual void flush() = 0;
private:
    AudioOutputBase& m_audio;
};

typedef BackgroundAudioWAVClass<uint8_t> ROMBackgroundAudioWAV;

#endif // MOCK_BACKGROUND_AUDIO_WAV_H
