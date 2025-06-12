#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Logger.h>
#include <AudioPlayer.h>
#include "AnimationPins.h"
#include "AnimationInputs.h"

// Helper function declarations
enum class MotorDirection : int8_t {
    Stop = 0,
    Forward = 1,
    Backward = -1
};

// Allow multiplication of MotorDirection with int
inline MotorDirection& operator*=(MotorDirection& lhs, int rhs) {
    return lhs = static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}

// Allow multiplication of int with MotorDirection
inline MotorDirection operator*(int lhs, MotorDirection rhs) {
    return static_cast<MotorDirection>(lhs * static_cast<int8_t>(rhs));
}

// Allow multiplication of MotorDirection with int
inline MotorDirection operator*(MotorDirection lhs, int rhs) {
    return static_cast<MotorDirection>(static_cast<int8_t>(lhs) * rhs);
}


class Animation {
public:
    Animation(Stream* serial,
        Adafruit_NeoPixel* pixels, 
        AudioPlayer* audio, 
        const AnimationPins& pins) : 
        m_pixels(pixels),
        m_audioPlayer(audio),
        m_logger(serial, "[Animation] "),
        m_pins(pins)
    {
    }

    void update(const AnimationInputs& inputs);
    virtual void setRotationDirection();
    virtual void performRotate();

    // Getters for testing
    int8_t getInputSensorLeft() const { return m_inputSensorLeft; }
    int8_t getInputSensorRight() const { return m_inputSensorRight; }
    int8_t getInputPIRSensor() const { return m_inputPIRSensor; }
    int8_t getInputButtonRectangle() const { return m_inputButtonRectangle; }
    int8_t getInputButtonCircle() const { return m_inputButtonCircle; }
    unsigned long getCurrentTime() const { return m_currentTime; }
    MotorDirection getMotorDirection() const { return m_motorDirection; }
    unsigned long getRandomRotateTimer() const { return m_randomRotateTimer; }
    unsigned long getLastLeftTurnTime() const { return m_lastLeftTurnTime; }
    unsigned long getLastRightTurnTime() const { return m_lastRightTurnTime; }
    unsigned long getLastPIRTimer() const { return m_lastPIRTimer; }
    bool getIsRainbowActive() const { return m_isRainbowActive; }
    uint8_t getRainbowIndex() const { return m_rainbowIndex; }
    unsigned long getRainbowTimer() const { return m_rainbowTimer; }

    // Setters for testing
    void setInputSensorLeft(int8_t value) { m_inputSensorLeft = value; }
    void setInputSensorRight(int8_t value) { m_inputSensorRight = value; }
    void setInputPIRSensor(int8_t value) { m_inputPIRSensor = value; }
    void setInputButtonRectangle(int8_t value) { m_inputButtonRectangle = value; }
    void setInputButtonCircle(int8_t value) { m_inputButtonCircle = value; }
    void setCurrentTime(unsigned long value) { m_currentTime = value; }
    void setMotorDirection(MotorDirection value) { m_motorDirection = value; }
    void setRandomRotateTimer(unsigned long value) { m_randomRotateTimer = value; }
    void setLastLeftTurnTime(unsigned long value) { m_lastLeftTurnTime = value; }
    void setLastRightTurnTime(unsigned long value) { m_lastRightTurnTime = value; }
    void setLastPIRTimer(unsigned long value) { m_lastPIRTimer = value; }
    void setIsRainbowActive(bool value) { m_isRainbowActive = value; }
    void setRainbowIndex(uint8_t value) { m_rainbowIndex = value; }
    void setRainbowTimer(unsigned long value) { m_rainbowTimer = value; }

    virtual void eyeBlink();
    virtual void updateSound();
    virtual void rotate(uint8_t speed, MotorDirection direction);
    virtual void stop();
    virtual uint32_t wheel(uint8_t pos);
    virtual void updateRainbow();

protected:
    AnimationPins m_pins;
    Adafruit_NeoPixel* m_pixels = nullptr;
    Logger m_logger;
    AudioPlayer* m_audioPlayer = nullptr;
    MotorDirection m_motorDirection = MotorDirection::Stop;
    int8_t m_inputSensorLeft;
    int8_t m_inputSensorRight;
    int8_t m_inputPIRSensor;
    int8_t m_inputButtonRectangle;
    int8_t m_inputButtonCircle;
    unsigned long m_lastPIRTimer;
    unsigned long m_randomRotateTimer;
    unsigned long m_lastLeftTurnTime;
    unsigned long m_lastRightTurnTime;

    uint8_t m_rainbowIndex;  // Current position in rainbow cycle
    unsigned long m_rainbowTimer; // Timer for rainbow animation
    bool m_isRainbowActive;   // Flag to track rainbow state
    unsigned long m_currentTime;      // Timestamp from update() call

};

#endif // ANIMATION_H