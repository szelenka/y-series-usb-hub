#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SleepyDog.h>
#include <Wire.h>

#include "Animation.h"
#include "AnimationInputs.h"
#include "EyeAnimation.h"
#include "Logger.h"
#include <WavData.h>
#include <TimerAudio.h>

// https://github.com/adafruit/Adafruit-KB2040-PCB/blob/main/Adafruit%20KB2040%20Pinout.pdf
#define PIN_PIR_SENSOR 3        // 3  --> PIR (yellow wire)
#define PIN_DOME_LED_GREEN 4    // 4  --> LED (green wire)
#define PIN_DOME_LED_BLUE 4     // 4  --> LED
#define PIN_EYE_NEOPIXEL 5      // 5  --> DATA IN (purple wire)
#define PIN_BUTTON_RECTANGLE 6  // 6  --> BUTTON
#define PIN_BUTTON_CIRCLE 7     // 7  --> BUTTON
#define PIN_SENSOR_LEFT 8       // 8  --> HALL
#define PIN_SENSOR_RIGHT 9      // 9  --> HALL
#define PIN_AMP_SHDWM 10        // 10 --> AMP SHDWM
#define PIN_NECK_MOTOR_IN1 26   // A0 --> MOT ANI2
#define PIN_NECK_MOTOR_IN2 27   // A1 --> MOT ANI1
#define PIN_AUDIO_OUT_NEG 28    // A2 --> AMP A-
#define PIN_AUDIO_OUT_POS 29    // A3 --> AMP A+

#define NUMPIXELS 17

// Create AnimationPins with custom pin values
AnimationPins customPins(PIN_EYE_NEOPIXEL, PIN_NECK_MOTOR_IN1, PIN_NECK_MOTOR_IN2, PIN_SENSOR_LEFT,
                         PIN_SENSOR_RIGHT, PIN_PIR_SENSOR, PIN_BUTTON_RECTANGLE, PIN_BUTTON_CIRCLE,
                         PIN_AUDIO_OUT_POS, PIN_AUDIO_OUT_NEG, PIN_DOME_LED_GREEN,
                         PIN_DOME_LED_BLUE);

Adafruit_NeoPixel neoPixel(NUMPIXELS, customPins.eyeNeck, NEO_GRB + NEO_KHZ800);
EyeAnimation eyeAnimation(&neoPixel);
TimerAudio timerAudio(customPins.audioOutPos, customPins.audioOutNeg);
AudioPlayer audioPlayer(&timerAudio);

Animation animation(&eyeAnimation, &audioPlayer, customPins);

void setup()
{
    // Initialize Serial for Logger
    Serial.begin(115200);
    delay(1000);

    Log.setLogLevel(LogLevel::INFO);
    Log.raw("Starting up...");

    // LED Setup
    pinMode(customPins.domeLedGreen, OUTPUT);
    pinMode(customPins.domeLedBlue, OUTPUT);

    // Neopixel Setup
    neoPixel.begin();
    neoPixel.clear();
    neoPixel.show();
    eyeAnimation.setTopPixels(5, 4);
    eyeAnimation.setCurrentTime(millis());
    eyeAnimation.blink(300);

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

    // Audio Setup
    pinMode(PIN_AMP_SHDWM, OUTPUT);
    pinMode(PIN_AUDIO_OUT_POS, OUTPUT);
    pinMode(PIN_AUDIO_OUT_NEG, OUTPUT);
    digitalWrite(PIN_AMP_SHDWM, HIGH);
    analogWrite(customPins.audioOutPos, 255);
    timerAudio.begin();
    audioPlayer.play(4);
}

static uint8_t nextSoundIndex = 1;
void loop()
{
    // Read sensor inputs
    AnimationInputs inputs = readInputs(customPins);

    Log.debug("Sensors: L%d R%d P%d B%d C%d", inputs.sensorLeft, inputs.sensorRight,
              inputs.pirSensor, inputs.buttonRectangle, inputs.buttonCircle);

    static unsigned long lastAudioPlayTime = 0;
    static unsigned long lastToggleTime = 0;

    // Once every 3 seconds, play a 1-second tone
    if (inputs.buttonRectangle == LOW && inputs.buttonCircle == LOW) {
        animation.stop();
        if (!timerAudio.isPlaying()) {
            timerAudio.playWAV(nextSoundIndex++);
            nextSoundIndex = nextSoundIndex % NUM_SOUND_FILES;
        }
    }
    // Update animation
    animation.update(inputs);
    animation.performRotate();
    animation.eyeBlink();
    animation.updateSound();

    // Sleep for 10ms - this is more power efficient than delay
    Watchdog.sleep(10);
}
