#ifndef DOUGH_BUTTON_H
#define DOUGH_BUTTON_H

#define BUTTON_DEBOUNCE_DELAY 50
#define BUTTON_LONGPRESS_DELAY 1000

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

    typedef void (*ButtonHandler)();

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
        Button(int pin);
        void setup();
        void loop();
        void onInterrupt(ButtonHandler isr);
        void onPress(ButtonHandler handler);
        void onShortPress(ButtonHandler handler);
        void onLongPress(ButtonHandler handler);
        void clearEvents();
        void handleButtonState();

    private:
        int _pin;
        ButtonHandler _pressHandler = nullptr;
        ButtonHandler _shortPressHandler = nullptr;
        ButtonHandler _longPressHandler = nullptr;
        bool _debounceState = false;
        unsigned long _debounceTimer = 0;
        ButtonState _state = UP;
    };
}

#endif