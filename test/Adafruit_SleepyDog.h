#ifndef MOCK_ADAFRUIT_SLEEPSYDOG_H
#define MOCK_ADAFRUIT_SLEEPSYDOG_H

class WatchdogType {
public:
    void begin() {}
    void enable() {}
    void disable() {}
    void reset() {}
    void setPeriod(uint32_t period) {}
    void setAction(uint8_t action) {}
    void sleep(uint32_t period) {}
};

extern WatchdogType Watchdog;

#endif // MOCK_ADAFRUIT_SLEEPSYDOG_H
