#ifndef MOCK_BACKGROUND_AUDIO_WAV_H
#define MOCK_BACKGROUND_AUDIO_WAV_H

#include "AudioOutputBase.h"

template<typename T>
class BackgroundAudioWAVClass {
public:
    BackgroundAudioWAVClass(AudioOutputBase& audio) : m_audio(audio) {}
    
    virtual void begin() {};
    virtual void end() {};
    virtual void play() {};
    virtual void stop() {};
    virtual bool playing() { return false; };
    virtual void write(const uint8_t* data, size_t size) {};
    virtual void write(uint8_t c) {};
    virtual void flush() {};
private:
    AudioOutputBase& m_audio;
};

typedef BackgroundAudioWAVClass<uint8_t> ROMBackgroundAudioWAV;

#endif // MOCK_BACKGROUND_AUDIO_WAV_H
