#include "LED.h"

namespace Dough
{
    LED::LED(int pin) : _pin(pin) {}

    void LED::setup()
    {
        pinMode(_pin, OUTPUT);
        _state = OFF;
        _setPin(LOW);
    }

    void LED::loop()
    {
        unsigned long now = millis();
        bool tick = (now - _timer) > _time;

        if (_state == FLASH)
        {
            if (tick)
            {
                _setPin(LOW);
                _state = OFF;
            }
        }
        else if (_state == DIP)
        {
            if (tick)
            {
                _setPin(HIGH);
                _state = ON;
            }
        }
        else if (_state == BLINK_ON)
        {
            if (_blinkStep == _blinkOnStep)
            {
                _setPin(HIGH);
            }
            if (tick)
            {
                _setPin(LOW);
                _state = BLINK_OFF;
                _timer = now;
            }
        }
        else if (_state == BLINK_OFF)
        {
            if (tick)
            {
                _state = BLINK_ON;
                _timer = now;
                _blinkStep++;
                if (_blinkStep > _blinkOfSteps)
                {
                    _blinkStep = 1;
                }
            }
        }
        else if (_state == PULSE)
        {
            if (tick)
            {
                _timer = now;
                _time = 1;
                _brightness += _pulseStep;
                if (_brightness <= 0)
                {
                    _time = 200;
                    _brightness = 0;
                    _pulseStep = -_pulseStep;
                }
                else if (_brightness >= 100)
                {
                    _brightness = 100;
                    _pulseStep = -_pulseStep;
                }
            }
            analogWrite(_pin, _brightness);
        }
        else if (_state == OFF)
        {
            _setPin(LOW);
        }
        else if (_state == ON)
        {
            _setPin(HIGH);
        }
    }

    void LED::_setPin(int high_or_low)
    {
        _pinState = high_or_low;
        analogWrite(_pin, _pinState == LOW ? 0 : 255);
    }

    void LED::on()
    {
        _state = ON;
        loop();
    }

    void LED::off()
    {
        _state = OFF;
        loop();
    }

    LED *LED::flash()
    {
        _setPin(HIGH);
        _state = FLASH;
        _timer = millis();
        _time = LED_TRANSITION_TIME_DEFAULT;
        loop();
        return this;
    }

    LED *LED::blink()
    {
        return blink(1, 1);
    }

    LED *LED::dip()
    {
        _setPin(LOW);
        _state = DIP;
        _timer = millis();
        _time = LED_TRANSITION_TIME_DEFAULT;
        loop();
        return this;
    }

    LED *LED::blink(int onStep, int ofSteps)
    {
        _blinkOnStep = onStep;
        _blinkOfSteps = ofSteps;
        _blinkStep = 1;
        _state = BLINK_ON;
        _time = LED_TRANSITION_TIME_DEFAULT;
        loop();
        return this;
    }

    void LED::pulse()
    {
        _state = PULSE;
        _brightness = 0;
        _pulseStep = +8;
        _time = 1;
    }

    void LED::slow()
    {
        _time = LED_TRANSITION_TIME_SLOW;
    }

    void LED::fast()
    {
        _time = LED_TRANSITION_TIME_FAST;
    }

    bool LED::isOn()
    {
        return _pinState == HIGH;
    }

    bool LED::isOff()
    {
        return _pinState == LOW;
    }
} // namespace Dough