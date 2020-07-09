#ifndef DOUGH_UI_H
#define DOUGH_UI_H

#define LOG_BAUDRATE 9600

// Define this one to wait for USB serial to come up.
// This can be useful during development, when you want all
// serial messages to appear in the serial monitor.
// Without this, some of the initial serial messages might
// be missing from the output.
#undef LOG_WAIT_SERIAL

#include <Arduino.h>
#include <WiFiNINA.h>
#include <stdarg.h>
#include "UI/DoughButton.h"
#include "UI/DoughLED.h"
#include "config.h"

class DoughUI
{
public:
    static DoughUI *Instance();
    void setup();
    static void onoffButtonISR();
    static void setupButtonISR();
    DoughButton onoffButton;
    DoughButton setupButton;
    DoughLED ledBuiltin;
    DoughLED led1;
    DoughLED led2;
    DoughLED led3;
    void processButtonEvents();
    void clearButtonEvents();
    void updatedLEDs();
    void flash_all_leds();
    void resume();
    void suspend();
    void log(const char *category, const char *fmt, ...);

private:
    DoughUI();
    void _setupTimerInterrupt();
    static DoughUI *_instance;
};

#endif
