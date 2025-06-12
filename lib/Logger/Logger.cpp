#include "Logger.h"
#include <cstdio>
#include <stdarg.h> 

// Initialize static member
bool Logger::s_enabled = true;

void Logger::print(const char* format, ...)
{
    if (!s_enabled || m_serial == nullptr) {
        return;
    }
    char buffer[128];
    
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    m_serial->print(m_prefix);
    m_serial->println(buffer);
}
