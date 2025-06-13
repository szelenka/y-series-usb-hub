/**
 * @file AnimationInputs.h
 * @brief Input state management for the animation system
 *
 * This file defines the AnimationInputs structure that holds the current state
 * of all input devices used in the animation system, including sensors and buttons.
 * It also provides a utility function to read these inputs from hardware pins.
 */

#ifndef ANIMATIONINPUTS_H
#define ANIMATIONINPUTS_H

#include <Arduino.h>

#include "AnimationPins.h"

/**
 * @brief Holds the current state of all animation input devices
 *
 * This structure stores the digital state of all input devices at a given moment,
 * along with a timestamp. It's used to pass input state between different
 * components of the animation system.
 */
struct AnimationInputs
{
    int8_t sensorLeft;       ///< Current state of the left hall effect sensor (HIGH/LOW)
    int8_t sensorRight;      ///< Current state of the right hall effect sensor (HIGH/LOW)
    int8_t pirSensor;        ///< Current state of the PIR motion sensor (HIGH/LOW)
    int8_t buttonRectangle;  ///< Current state of the rectangular button (HIGH/LOW, with pull-up:
                             ///< LOW when pressed)
    int8_t buttonCircle;     ///< Current state of the circular button (HIGH/LOW, with pull-up: LOW
                             ///< when pressed)
    unsigned long currentTime;  ///< Timestamp in milliseconds when inputs were read
};

/**
 * @brief Reads the current state of all input devices
 *
 * This helper function reads the digital state of all input devices using the
 * provided pin configuration and returns them in an AnimationInputs structure.
 * The function also records the current time using millis().
 *
 * @param pins Reference to an AnimationPins structure containing pin numbers
 * @return AnimationInputs Structure containing the current state of all inputs
 *                         and the timestamp of when they were read
 */
inline AnimationInputs readInputs(const AnimationPins& pins)
{
    AnimationInputs inputs;
    inputs.sensorLeft = digitalRead(pins.sensorLeft);
    inputs.sensorRight = digitalRead(pins.sensorRight);
    inputs.pirSensor = digitalRead(pins.pirSensor);
    inputs.buttonRectangle = digitalRead(pins.buttonRectangle);
    inputs.buttonCircle = digitalRead(pins.buttonCircle);
    inputs.currentTime = millis();
    return inputs;
}

#endif  // ANIMATIONINPUTS_H
