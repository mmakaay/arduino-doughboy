#include "DoughLED.h"

DoughLED::DoughLED(int pin) {
    _pin = pin;
}

void DoughLED::setup() {
    pinMode(_pin, OUTPUT);
    _state = OFF;
    _setPin(LOW);
}

void DoughLED::loop() {
    unsigned long now = millis();
    bool tick = (now - _timer) > _time;
    
    if (_state == FLASH) {
        if (tick) {
            _setPin(LOW);
            _state = OFF;
        }
    }
    else if (_state == DIP) {
        if (tick) {
            _setPin(HIGH);
            _state = ON;
        }
    }
    else if (_state == BLINK_ON) {
        if (_blinkStep == _blinkOnStep) {
            _setPin(HIGH);
        }
        if (tick) {
            _setPin(LOW);
            _state = BLINK_OFF;
            _timer = now;
        }
    }
    else if (_state == BLINK_OFF) {
        if (tick) {
            _state = BLINK_ON;
            _timer = now;
            _blinkStep++;
            if (_blinkStep > _blinkOfSteps) {
                _blinkStep = 1;
            }
        }
    }    
    else if (_state == PULSE) {
        if (tick) {
            _timer = now;
            _time = 1;
            _brightness += _pulseStep;
            if (_brightness <= 0) {
                _time = 200;
                _brightness = 0;
                _pulseStep = -_pulseStep;
            }
            else if (_brightness >= 100) {
                _brightness = 100;
                _pulseStep = -_pulseStep;
            }
        }
        analogWrite(_pin, _brightness);
    }
    else if (_state == OFF) {
        _setPin(LOW);
    }
    else if (_state == ON) {
        _setPin(HIGH);
    }
}

void DoughLED::_setPin(int high_or_low) {
    _pinState = high_or_low;
    analogWrite(_pin, _pinState == LOW ? 0 : 255);    
}

void DoughLED::on() {
    _state = ON;
    loop();
}

void DoughLED::off() {
    _state = OFF;
    loop();
}

DoughLED* DoughLED::flash() {
    _setPin(HIGH);
    _state = FLASH;
    _timer = millis();
    _time = LED_TRANSITION_TIME_DEFAULT;
    loop();
    return this;
}

DoughLED* DoughLED::blink() {
    return blink(1, 1);
}

DoughLED* DoughLED::dip() {
    _setPin(LOW);
    _state = DIP;
    _timer = millis();
    _time = LED_TRANSITION_TIME_DEFAULT;
    loop();
    return this;
}

DoughLED* DoughLED::blink(int onStep, int ofSteps) {
    _blinkOnStep = onStep;
    _blinkOfSteps = ofSteps;
    _blinkStep = 1;
    _state = BLINK_ON;
    _time = LED_TRANSITION_TIME_DEFAULT;
    loop();
    return this;
}

void DoughLED::pulse() {
    _state = PULSE;
    _brightness = 0;
    _pulseStep = +8;
    _time = 1;
}

void DoughLED::slow() {
    _time = LED_TRANSITION_TIME_SLOW;
}

void DoughLED::fast() {
    _time = LED_TRANSITION_TIME_FAST;
}

bool DoughLED::isOn() {
    return _pinState == HIGH;
}

bool DoughLED::isOff() {
    return _pinState == LOW;
}
