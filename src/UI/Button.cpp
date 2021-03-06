#include "Button.h"

namespace Dough
{
    // Constructor for a button instance.
    //
    // As a necessary evil, because of the way attachinterrupt() works in
    // Arduino, construction needs a bit of extra work to get the button
    // working. An interrupt service routine (ISR) function must be created
    // and linked to the button to get the interrupts working. Pattern:
    //
    //   // A function for handling interrupts.
    //   void myButtonISR() {
    //       myButton.handleButtonState();
    //   }
    //
    //   // Construct the button instance.
    //   Button myButton(MYBUTTON_PIN, myButtonISR);
    Button::Button(int pin, ButtonISR isr) : _pin(pin), _isr(isr) {}

    void Button::setup()
    {
        pinMode(_pin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_pin), _isr, CHANGE);
    }

    // Assign an event handler for short and long button presses.
    // When specific handlers for long and/or short presses are
    // configured as well, those have precedence over this one.
    void Button::onPress(ButtonISR handler)
    {
        _pressHandler = handler;
    }

    // Assign an event handler for long button presses.
    void Button::onLongPress(ButtonISR handler)
    {
        _longPressHandler = handler;
    }

    // Assign an event handler for short button presses.
    void Button::onShortPress(ButtonISR handler)
    {
        _shortPressHandler = handler;
    }

    void Button::loop()
    {
        handleButtonState();
        if (_state == UP_AFTER_SHORT)
        {
            if (_shortPressHandler != nullptr)
            {
                _shortPressHandler();
            }
            else if (_pressHandler != nullptr)
            {
                _pressHandler();
            }
            _state = READY_FOR_NEXT_PRESS;
        }
        else if (_state == DOWN_LONG || _state == UP_AFTER_LONG)
        {
            if (_longPressHandler != nullptr)
            {
                _longPressHandler();
            }
            else if (_pressHandler != nullptr)
            {
                _pressHandler();
            }
            _state = READY_FOR_NEXT_PRESS;
        }
        else if (_state == DOWN && _shortPressHandler == nullptr && _longPressHandler == nullptr)
        {
            if (_pressHandler != nullptr)
            {
                _pressHandler();
            }
            _state = READY_FOR_NEXT_PRESS;
        }
    }

    void Button::clearEvents()
    {
        _state = READY_FOR_NEXT_PRESS;
    }

    void Button::handleButtonState()
    {
        bool buttonIsDown = digitalRead(_pin) == 0;
        bool buttonIsUp = !buttonIsDown;

        // When the button state has changed since the last time, then
        // restart the debounce timer.
        if (buttonIsDown != _debounceState)
        {
            _debounceTimer = millis();
            _debounceState = buttonIsDown;
        }

        unsigned long interval = (millis() - _debounceTimer);

        // Handle button state changes.
        if (_state == READY_FOR_NEXT_PRESS && buttonIsUp)
        {
            _state = UP;
        }
        else if (_state == UP && buttonIsDown && interval)
        {
            _state = DOWN;
        }
        else if (_state == DOWN && buttonIsDown && interval > BUTTON_LONGPRESS_DELAY)
        {
            _state = DOWN_LONG;
        }
        else if (_state == DOWN && buttonIsUp && interval > BUTTON_DEBOUNCE_DELAY)
        {
            _state = UP_AFTER_SHORT;
        }
        else if (_state == DOWN_LONG && buttonIsUp)
        {
            _state = UP_AFTER_LONG;
        }
    }
} // namespace Dough