/**
 * @file AnimationPins.h
 * @brief Pin configuration structure for the animation controller
 * 
 * This file defines the AnimationPins structure that holds pin assignments
 * for all hardware components used in the animation system.
 */

#ifndef ANIMATION_PINS_H
#define ANIMATION_PINS_H

#include <Arduino.h>
#include <stdint.h>

/**
 * @brief Holds pin assignments for all animation-related hardware components
 * 
 * This structure contains pin numbers for all the hardware components
 * used in the animation system, including sensors, motors, and indicators.
 * It provides both default and custom constructors for easy initialization.
 */
struct AnimationPins
{
    const uint8_t eyeNeck;         ///< Pin number for the eye/neck servo control
    const uint8_t neckMotorIn1;     ///< First control pin for the neck motor (H-bridge input 1)
    const uint8_t neckMotorIn2;     ///< Second control pin for the neck motor (H-bridge input 2)
    const uint8_t sensorLeft;       ///< Left-side sensor pin for detecting movement or proximity
    const uint8_t sensorRight;      ///< Right-side sensor pin for detecting movement or proximity
    const uint8_t pirSensor;        ///< PIR (Passive Infrared) motion sensor pin
    const uint8_t buttonRectangle;  ///< Rectangular button input pin
    const uint8_t buttonCircle;     ///< Circular button input pin
    const uint8_t audioOut;         ///< Audio output pin for sound effects
    const uint8_t domeLedGreen;     ///< Green LED in the dome for status indication
    const uint8_t domeLedBlue;      ///< Blue LED in the dome for status indication

    /**
     * @brief Default constructor with standard pin assignments
     * 
     * Initializes all pins with default values:
     * - eyeNeck: 1
     * - neckMotorIn1: 2
     * - neckMotorIn2: 3
     * - sensorLeft: 4
     * - sensorRight: 5
     * - pirSensor: 6
     * - buttonRectangle: 7
     * - buttonCircle: 8
     * - audioOut: 9
     * - domeLedGreen: 10
     * - domeLedBlue: 11
     */
    AnimationPins()
        : eyeNeck(1),
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
    {
    }

    /**
     * @brief Custom constructor with specified pin assignments
     * 
     * @param eyeNeck Pin for eye/neck servo control
     * @param neckMotorIn1 First control pin for neck motor (H-bridge input 1)
     * @param neckMotorIn2 Second control pin for neck motor (H-bridge input 2)
     * @param sensorLeft Left-side sensor pin
     * @param sensorRight Right-side sensor pin
     * @param pirSensor PIR motion sensor pin
     * @param buttonRectangle Rectangular button input pin
     * @param buttonCircle Circular button input pin
     * @param audioOut Audio output pin
     * @param domeLedGreen Green LED in dome
     * @param domeLedBlue Blue LED in dome
     */
    AnimationPins(uint8_t eyeNeck, uint8_t neckMotorIn1, uint8_t neckMotorIn2, uint8_t sensorLeft,
                  uint8_t sensorRight, uint8_t pirSensor, uint8_t buttonRectangle,
                  uint8_t buttonCircle, uint8_t audioOut, uint8_t domeLedGreen, uint8_t domeLedBlue)
        : eyeNeck(eyeNeck),
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
    {
    }
};

#endif  // ANIMATION_PINS_H
