// test/Adafruit_NeoPixel.h
#ifndef ADAFRUIT_NEOPIXEL_H
#define ADAFRUIT_NEOPIXEL_H

#define NEO_KHZ800 0x0000 ///< 800 KHz data transmission
#define NEO_RGB ((0 << 6) | (0 << 4) | (1 << 2) | (2)) ///< Transmit as R,G,B
#define NEO_RBG ((0 << 6) | (0 << 4) | (2 << 2) | (1)) ///< Transmit as R,B,G
#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2)) ///< Transmit as G,R,B
#define NEO_GBR ((2 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,R
#define NEO_BRG ((1 << 6) | (1 << 4) | (2 << 2) | (0)) ///< Transmit as B,R,G
#define NEO_BGR ((2 << 6) | (2 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,R

class Adafruit_NeoPixel {
public:
    Adafruit_NeoPixel(uint16_t n, uint8_t p, uint8_t t) : numLEDs(n) {}
    
    virtual void begin() = 0 ;
    virtual void show() = 0 ;
    virtual void setPixelColor(uint16_t n, uint32_t c) = 0 ;
    virtual void clear() = 0 ;
    
    virtual uint16_t numPixels() const { return numLEDs; }
    
private:
    uint16_t numLEDs;
};

#endif // ADAFRUIT_NEOPIXEL_H
