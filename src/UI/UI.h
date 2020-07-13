#ifndef DOUGH_UI_H
#define DOUGH_UI_H

#include <Arduino.h>
#include <stdarg.h>
#include "UI/Button.h"
#include "UI/LED.h"
#include "config.h"

namespace Dough
{
// This class groups all user interface functionality: serial logging,
// LEDs and buttons.
class UI
{
public:
    static UI *Instance();
    void setup();
    static void onoffButtonISR();
    static void setupButtonISR();
    Button onoffButton;
    Button setupButton;
    LED ledBuiltin;
    LED led1;
    LED led2;
    LED led3;
    void processButtonEvents();
    void clearButtonEvents();
    void updatedLEDs();
    void resume();
    void suspend();

private:
    UI();
    void _setupTimerInterrupt();
    static UI *_instance;
    void _flash_all_leds();
};
}

#endif