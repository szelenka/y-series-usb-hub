#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

class Logger {
public:
    static bool s_enabled;

    Logger(Stream* serial, const char* prefix = "[Animation] ") :
        m_serial(serial), 
        m_prefix(prefix)
    {
    }
    
    void print(const char* format, ...);
    
private:
    Stream* m_serial = nullptr;
    const char* m_prefix;
};

#endif // LOGGER_H
