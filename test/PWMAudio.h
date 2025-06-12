#ifndef MOCK_PWM_AUDIO_H
#define MOCK_PWM_AUDIO_H

#include <Arduino.h>

#include "AudioOutputBase.h"

class PWMAudio : public AudioOutputBase
{
public:
    PWMAudio(uint8_t pin) : m_pin(pin) { pinMode(pin, OUTPUT); }

    bool begin() override { return true; }
    bool end() override { return true; }
    void write(const void* data, size_t size) {}
    void flush() {}
    bool playing() { return false; }
    void setVolume(uint8_t volume) {}

private:
    uint8_t m_pin;
};

#endif  // MOCK_PWM_AUDIO_H
