#ifndef DOUGH_BUTTON_H
#define DOUGH_BUTTON_H

#define BUTTON_DEBOUNCE_DELAY 50
#define BUTTON_LONGPRESS_DELAY 1000

#include <Arduino.h>
#include "config.h"

typedef enum {
    UP,
    DOWN,
    DOWN_LONG,
    UP_AFTER_LONG,
    UP_AFTER_SHORT,
    READY_FOR_NEXT_PRESS
} DoughButtonState;

typedef void (*DoughButtonHandler)();

class DoughButton {
    public:
        DoughButton(int pin);
        void setup();
        void loop();
        void onInterrupt(DoughButtonHandler isr);
        void onPress(DoughButtonHandler handler);
        void onShortPress(DoughButtonHandler handler);
        void onLongPress(DoughButtonHandler handler);
        void clearEvents();
        void handleButtonState();
    private:
        int _pin;
        DoughButtonHandler _pressHandler = nullptr;
        DoughButtonHandler _shortPressHandler = nullptr;
        DoughButtonHandler _longPressHandler = nullptr;
        bool _debounceState = false;
        unsigned long _debounceTimer = 0;
        DoughButtonState _state = UP;
};

#endif
