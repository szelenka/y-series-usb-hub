#ifndef MOCK_BACKGROUND_AUDIO_WAV_H
#define MOCK_BACKGROUND_AUDIO_WAV_H

#include "AudioOutputBase.h"

template <typename T>
class BackgroundAudioWAVClass
{
public:
    BackgroundAudioWAVClass(AudioOutputBase& audio) : m_audio(audio) {}

    virtual bool begin() { return true; };
    virtual void end() {};
    virtual void play() {};
    virtual void stop() {};
    virtual bool playing() { return false; };
    virtual size_t write(const uint8_t* data, size_t size) { return size; };
    virtual size_t write(uint8_t c) { return 1; };
    virtual void flush() {};

private:
    AudioOutputBase& m_audio;
};

typedef BackgroundAudioWAVClass<uint8_t> ROMBackgroundAudioWAV;

#endif  // MOCK_BACKGROUND_AUDIO_WAV_H
