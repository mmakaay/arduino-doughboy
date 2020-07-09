#include "DoughButton.h"

/**
 * Constructor for a button instance.
 * As a necessary evil, because of the way attachinterrupt() works in
 * Arduino, construction needs a bit of extra work to get the button
 * working. An interrupt service routine (ISR) function must be created
 * and linked to the button to get the interrupts working. Pattern:
 * 
 *   // Construct the button instance.
 *   DoughButton myButton(MYBUTTON_PIN);
 *   
 *   // A function for handling interrupts.
 *   void myButtonISR() {
 *       myButton.handleButtonState();
 *   }
 *   
 *   // Linking the function ot button interrupts.
 *   myButton.onInterrupt(myButtonISR);
 */
DoughButton::DoughButton(int pin)
{
    _pin = pin;
}

void DoughButton::setup()
{
    pinMode(_pin, INPUT_PULLUP);
}

/**
 * Assign an interrupt service routine (ISR) for handling button
 * interrupts. The provided isr should relay interrupts to the
 * handleButtonState() method of this class (see constructor docs).
 */
void DoughButton::onInterrupt(DoughButtonHandler isr)
{
    attachInterrupt(digitalPinToInterrupt(_pin), isr, CHANGE);
}

/**
 * Assign an event handler for short and long button presses.
 * When specific handlers for long and/or short presses are
 * configured as well, those have precedence over this one.
 */
void DoughButton::onPress(DoughButtonHandler handler)
{
    _pressHandler = handler;
}

/**
 * Assign an event handler for long button presses.
 */
void DoughButton::onLongPress(DoughButtonHandler handler)
{
    _longPressHandler = handler;
}

/**
 * Assign an event handler for short button presses.
 */
void DoughButton::onShortPress(DoughButtonHandler handler)
{
    _shortPressHandler = handler;
}

void DoughButton::loop()
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

void DoughButton::clearEvents()
{
    _state = READY_FOR_NEXT_PRESS;
}

void DoughButton::handleButtonState()
{
    bool buttonIsDown = digitalRead(_pin) == 0;
    bool buttonIsUp = !buttonIsDown;

    // When the button state has changed since the last time, then
    // start the debounce timer.
    if (buttonIsDown != _debounceState)
    {
        _debounceTimer = millis();
        _debounceState = buttonIsDown;
    }

    unsigned long interval = (millis() - _debounceTimer);

    // Only when the last state change has been stable for longer than the
    // configured debounce delay, then we accept the current state as
    // a stabilized button state.
    if (interval < BUTTON_DEBOUNCE_DELAY)
    {
        return;
    }

    // Handle button state changes.
    if (_state == READY_FOR_NEXT_PRESS && buttonIsUp)
    {
        _state = UP;
    }
    else if (_state == UP && buttonIsDown)
    {
        _state = DOWN;
    }
    else if (_state == DOWN && buttonIsDown && interval > BUTTON_LONGPRESS_DELAY)
    {
        _state = DOWN_LONG;
    }
    else if (_state == DOWN && buttonIsUp)
    {
        _state = UP_AFTER_SHORT;
    }
    else if (_state == DOWN_LONG && buttonIsUp)
    {
        _state = UP_AFTER_LONG;
    }
}
