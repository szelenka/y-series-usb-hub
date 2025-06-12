#ifndef MOCK_AUDIOOUTPUTBASE_H
#define MOCK_AUDIOOUTPUTBASE_H

#include <Arduino.h>

// Mock AudioOutputBase class
class AudioOutputBase
{
public:
    virtual ~AudioOutputBase() {}
    virtual bool setBuffers(size_t buffers, size_t bufferWords, int32_t silenceSample = 0)
    {
        return true;
    }
    virtual bool setBitsPerSample(int bps) { return true; }
    virtual bool setFrequency(int freq) { return true; }
    virtual bool setStereo(bool stereo = true) { return true; }
    virtual bool begin() { return true; }
    virtual bool end() { return true; }
    virtual bool getUnderflow() { return false; }
    virtual void onTransmit(void (*)(void*), void*) {}
    virtual size_t write(const uint8_t* buffer, size_t size) { return size; }
    virtual int availableForWrite() { return 1024; }
};

#endif  // MOCK_AUDIOOUTPUTBASE_H
