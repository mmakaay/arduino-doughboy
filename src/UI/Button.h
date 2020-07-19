#ifndef DOUGH_BUTTON_H
#define DOUGH_BUTTON_H

#define BUTTON_DEBOUNCE_DELAY 50
#define BUTTON_LONGPRESS_DELAY 500

#include <Arduino.h>

namespace Dough
{
    typedef enum
    {
        UP,
        DOWN,
        DOWN_LONG,
        UP_AFTER_LONG,
        UP_AFTER_SHORT,
        READY_FOR_NEXT_PRESS
    } ButtonState;

    typedef void (*ButtonISR)();

    // This class provides a simple interface for handling button presses.
    // Only a few events are supported:
    //
    // - short press: a button up event, will not trigger after long press
    // - long press: when a button is held down for a while
    // - press: this is a button down event, which will only trigger if
    //   short press and long press events aren't used
    class Button
    {
    public:
        Button(int pin, ButtonISR isr);
        void setup();
        void loop();
        void onPress(ButtonISR handler);
        void onShortPress(ButtonISR handler);
        void onLongPress(ButtonISR handler);
        void clearEvents();
        void handleButtonState();

    private:
        int _pin;
        ButtonISR _isr;
        ButtonISR _pressHandler = nullptr;
        ButtonISR _shortPressHandler = nullptr;
        ButtonISR _longPressHandler = nullptr;
        bool _debounceState = false;
        unsigned long _debounceTimer = 0;
        ButtonState _state = UP;
    };
}

#endif