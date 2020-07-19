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
        UI(ButtonISR onoffButtonISR, ButtonISR setupButtonISR);
        void setup();
        Button onoffButton;
        Button setupButton;
        void processButtonEvents();
        void clearButtonEvents();
        void updateLEDs();
        void resume();
        void suspend();
        void notifyConnectingToWifi();
        void notifyConnectingToMQTT();
        void notifyConnected();
        void notifyWaitingForConfiguration();
        void notifyCalibrating();
        void notifyMeasurementsActive();
        void notifyMeasurementsPaused();
        void notifySensorActivity();
        void notifyNetworkActivity();

    private:
        LED _ledBuiltin;
        LED _led1;
        LED _led2;
        LED _led3;
        void _setupTimerInterrupt();
        static UI *_instance;
        void _flash_all_leds();
    };
} // namespace Dough

#endif