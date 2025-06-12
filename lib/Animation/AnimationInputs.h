#ifndef ANIMATIONINPUTS_H
#define ANIMATIONINPUTS_H

#include <Arduino.h>

struct AnimationInputs {
    int8_t sensorLeft;
    int8_t sensorRight;
    int8_t pirSensor;
    int8_t buttonRectangle;
    int8_t buttonCircle;
    unsigned long currentTime;
};

inline AnimationInputs readInputs(const AnimationPins& pins) {
    AnimationInputs inputs;
    inputs.sensorLeft = digitalRead(pins.sensorLeft);
    inputs.sensorRight = digitalRead(pins.sensorRight);
    inputs.pirSensor = digitalRead(pins.pirSensor);
    inputs.buttonRectangle = digitalRead(pins.buttonRectangle);
    inputs.buttonCircle = digitalRead(pins.buttonCircle);
    inputs.currentTime = millis();
    return inputs;
}

#endif // ANIMATIONINPUTS_H
