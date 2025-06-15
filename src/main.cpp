#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SleepyDog.h>
#include <BackgroundAudioWAV.h>
#include <PWMAudio.h>
#include <Wire.h>

#include "Animation.h"
#include "AnimationInputs.h"
#include "EyeAnimation.h"
#include "Logger.h"
#include <WavData.h>

// https://github.com/adafruit/Adafruit-KB2040-PCB/blob/main/Adafruit%20KB2040%20Pinout.pdf
#define PIN_PIR_SENSOR 3        // 3
#define PIN_DOME_LED_GREEN 4    // 4
#define PIN_DOME_LED_BLUE 4     // 4
#define PIN_EYE_NEOPIXEL 5      // 5
#define PIN_BUTTON_RECTANGLE 6  // 6
#define PIN_BUTTON_CIRCLE 7     // 7
#define PIN_SENSOR_LEFT 8       // 8
#define PIN_SENSOR_RIGHT 9      // 9
#define PIN_AMP_SD 10           // 10
#define PIN_NECK_MOTOR_IN1 26   // A0
#define PIN_NECK_MOTOR_IN2 27   // A1
#define PIN_AUDIO_OUT_NEG 28    // A2
#define PIN_AUDIO_OUT_POS 29    // A3

#define NUMPIXELS 17
#define NEOPIXEL_BRIGHTNESS 50
#define SAMPLE_RATE 22050
#define SAMPLE_INTERVAL 1000000 / SAMPLE_RATE
#define SAMPLE_BITRATE 16

// Create AnimationPins with custom pin values
AnimationPins customPins(PIN_EYE_NEOPIXEL, PIN_NECK_MOTOR_IN1, PIN_NECK_MOTOR_IN2, PIN_SENSOR_LEFT,
                         PIN_SENSOR_RIGHT, PIN_PIR_SENSOR, PIN_BUTTON_RECTANGLE, PIN_BUTTON_CIRCLE,
                         PIN_AUDIO_OUT_POS, PIN_AUDIO_OUT_NEG, PIN_DOME_LED_GREEN, PIN_DOME_LED_BLUE);

Adafruit_NeoPixel neoPixel(NUMPIXELS, customPins.eyeNeck, NEO_GRB + NEO_KHZ800);
EyeAnimation eyeAnimation(&neoPixel);
PWMAudio pwmAudio(customPins.audioOutPos, customPins.audioOutNeg);
ROMBackgroundAudioWAV bga(pwmAudio);
AudioPlayer audioPlayer(&bga);

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
    neoPixel.setBrightness(NEOPIXEL_BRIGHTNESS);
    eyeAnimation.setDefaultColor(0x00, 0x00, 0x80);
    eyeAnimation.setBrightness(NEOPIXEL_BRIGHTNESS);
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
    pinMode(PIN_AMP_SD, OUTPUT);
    digitalWrite(PIN_AMP_SD, HIGH);
    pwmAudio.begin(SAMPLE_RATE, SAMPLE_BITRATE);
    pwmAudio.setStereo(false);
    pinMode(customPins.audioOutPos, OUTPUT);
    pinMode(customPins.audioOutNeg, OUTPUT);
    audioPlayer.play(4);
}
size_t position = 0;
unsigned long lastSampleTime = 0;
void loop()
{
    // Update animation
    animation.update(readInputs(customPins));
    animation.performRotate();
    animation.eyeBlink();
    animation.updateSound();

    // Sleep for 10ms - this is more power efficient than delay
    Watchdog.sleep(10);
}
