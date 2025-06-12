#ifndef ANIMATION_PINS_H
#define ANIMATION_PINS_H

#include <Arduino.h>
#include <stdint.h>

struct AnimationPins {
    const uint8_t eyeNeck;
    const uint8_t neckMotorIn1;
    const uint8_t neckMotorIn2;
    const uint8_t sensorLeft;
    const uint8_t sensorRight;
    const uint8_t pirSensor;
    const uint8_t buttonRectangle;
    const uint8_t buttonCircle;
    const uint8_t audioOut;
    const uint8_t domeLedGreen;
    const uint8_t domeLedBlue;

    // Default constructor with standard values
    AnimationPins() :
        eyeNeck(1),
        neckMotorIn1(2),
        neckMotorIn2(3),
        sensorLeft(4),
        sensorRight(5),
        pirSensor(6),
        buttonRectangle(7),
        buttonCircle(8),
        audioOut(9),
        domeLedGreen(10),
        domeLedBlue(11)
    {}

    // Constructor with custom values
    AnimationPins(
        uint8_t eyeNeck,
        uint8_t neckMotorIn1,
        uint8_t neckMotorIn2,
        uint8_t sensorLeft,
        uint8_t sensorRight,
        uint8_t pirSensor,
        uint8_t buttonRectangle,
        uint8_t buttonCircle,
        uint8_t audioOut,
        uint8_t domeLedGreen,
        uint8_t domeLedBlue
    ) :
        eyeNeck(eyeNeck),
        neckMotorIn1(neckMotorIn1),
        neckMotorIn2(neckMotorIn2),
        sensorLeft(sensorLeft),
        sensorRight(sensorRight),
        pirSensor(pirSensor),
        buttonRectangle(buttonRectangle),
        buttonCircle(buttonCircle),
        audioOut(audioOut),
        domeLedGreen(domeLedGreen),
        domeLedBlue(domeLedBlue)
    {}
};

#endif // ANIMATION_PINS_H
