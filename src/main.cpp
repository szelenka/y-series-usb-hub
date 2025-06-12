#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SleepyDog.h>
#include <BackgroundAudioWAV.h>
#include <PWMAudio.h>
#include <Wire.h>

#include "Animation.h"
#include "AnimationInputs.h"

// https://github.com/adafruit/Adafruit-KB2040-PCB/blob/main/Adafruit%20KB2040%20Pinout.pdf
#define PIN_NECK_MOTOR_IN1      26 // A0
#define PIN_NECK_MOTOR_IN2      27 // A1
#define PIN_AUDIO_OUT           29 // A3
#define PIN_EYE_NEOPIXEL        5  // 5
#define PIN_DOME_LED_GREEN      2  // 2
#define PIN_DOME_LED_BLUE       2  // 2
#define PIN_SENSOR_LEFT         3  // 3
#define PIN_SENSOR_RIGHT        4  // 4
#define PIN_PIR_SENSOR          6  // 6
#define PIN_BUTTON_RECTANGLE    7  // 7
#define PIN_BUTTON_CIRCLE       8  // 8

#define NUMPIXELS 17
#define NEOPIXEL_BRIGHTNESS 50

// Create AnimationPins with custom pin values
AnimationPins customPins(PIN_EYE_NEOPIXEL, PIN_NECK_MOTOR_IN1, PIN_NECK_MOTOR_IN2, PIN_SENSOR_LEFT,
                         PIN_SENSOR_RIGHT, PIN_PIR_SENSOR, PIN_BUTTON_RECTANGLE, PIN_BUTTON_CIRCLE,
                         PIN_AUDIO_OUT, PIN_DOME_LED_GREEN, PIN_DOME_LED_BLUE);

Adafruit_NeoPixel neoPixel(NUMPIXELS, customPins.eyeNeck, NEO_GRB + NEO_KHZ800);
PWMAudio audio(customPins.audioOut);
ROMBackgroundAudioWAV bga(audio);
AudioPlayer audioPlayer(&bga);

Animation animation(&Serial, &neoPixel, &audioPlayer, customPins);

void setup()
{
    // LED Setup
    pinMode(customPins.domeLedGreen, OUTPUT);
    pinMode(customPins.domeLedBlue, OUTPUT);

    // Neopixel Setup
    neoPixel.begin();
    neoPixel.clear();
    neoPixel.show();
    neoPixel.setBrightness(NEOPIXEL_BRIGHTNESS);

    // Audio Setup
    pinMode(customPins.audioOut, OUTPUT);

    // Sensor Setup
    pinMode(customPins.sensorLeft, INPUT);
    pinMode(customPins.sensorRight, INPUT);
    pinMode(customPins.pirSensor, INPUT);

    // Button Setup
    pinMode(customPins.buttonRectangle, INPUT_PULLUP);
    pinMode(customPins.buttonCircle, INPUT_PULLUP);

    // Motor Setup
    pinMode(customPins.neckMotorIn1, OUTPUT);
    pinMode(customPins.neckMotorIn2, OUTPUT);
    analogWrite(customPins.neckMotorIn1, LOW);
    analogWrite(customPins.neckMotorIn2, LOW);
}

void loop()
{
    animation.update(readInputs(customPins));
    animation.setRotationDirection();
    animation.performRotate();
    animation.eyeBlink();
    animation.updateSound();
    animation.updateRainbow();

    // Sleep for 100ms - this is more power efficient than delay
    Watchdog.sleep(100);
}
