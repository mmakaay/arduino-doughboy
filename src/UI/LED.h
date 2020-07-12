#ifndef DOUGH_LED_H
#define DOUGH_LED_H

// Delay times for blinking, flashing and dipping.
#define LED_TRANSITION_TIME_SLOW 400
#define LED_TRANSITION_TIME_DEFAULT 250
#define LED_TRANSITION_TIME_FAST 100

#include <Arduino.h>

namespace Dough
{
    typedef enum
    {
        ON,
        OFF,
        BLINK_ON,
        BLINK_OFF,
        FLASH,
        DIP,
        PULSE
    } LEDState;

    // This class provides a set of basic LED lighting patterns, which can
    // be used in an async way.
    class LED
    {
    public:
        LED(int pin);
        void setup();
        void loop();
        void on();
        void off();
        LED *blink();
        LED *blink(int onStep, int ofSteps);
        LED *flash();
        LED *dip();
        void pulse();
        void slow();
        void fast();
        bool isOn();
        bool isOff();

    private:
        int _pin;
        int _pinState = LOW;
        LEDState _state = OFF;
        void _setPin(int high_or_low);
        unsigned long _timer;
        unsigned int _time;
        int _blinkOnStep;
        int _blinkOfSteps;
        int _blinkStep;
        int _brightness;
        int _pulseStep;
    };
} // namespace Dough

#endif