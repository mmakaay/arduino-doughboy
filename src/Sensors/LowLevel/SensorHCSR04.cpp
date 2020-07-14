#include "Sensors/LowLevel/SensorHCSR04.h"

namespace Dough
{
    SensorHCSR04::SensorHCSR04(int triggerPin, int echoPin) : _logger("HCSR04")
    {
        precision = 3;
        _triggerPin = triggerPin;
        _echoPin = echoPin;
        _temperature = HCSR04_INIT_TEMPERATURE;
        _humidity = HCSR04_INIT_HUMIDITY;
#ifndef HCSR04_DEBUG
        _logger.suspend();
#endif
    }

    void SensorHCSR04::setup()
    {
        _logger.log("sisi", "Setup output pin ", _triggerPin, " and input pin ", _echoPin);
        pinMode(_triggerPin, OUTPUT);
        pinMode(_echoPin, INPUT);
    }

    void SensorHCSR04::setTemperature(int temperature)
    {
        _logger.log("sis", "Set temperature to ", temperature, "°C");
        _temperature = temperature;
    }

    void SensorHCSR04::setHumidity(int humidity)
    {
        _logger.log("sis", "Set humidity to ", humidity, "%");
        _humidity = humidity;
    }

    // Get a distance reading.
    // When reading the distance fails, -1 is returned.
    // Otherwise the distance in mm.
    int SensorHCSR04::readDistance()
    {
        _setSpeedOfSound();
        _setEchoTimeout();
        _takeSamples();
        if (_haveEnoughSamples())
        {
            _sortSamples();
            return _computeAverage();
        }
        return -1;
    }

    // Sets the speed of sound in mm/Ms, depending on the temperature
    // and relative humidity. I derived this formula from a YouTube
    // video about the HC-SR04: https://youtu.be/6F1B_N6LuKw?t=1548
    void SensorHCSR04::_setSpeedOfSound()
    {
        _speedOfSound =
            0.3314 +
            (0.000606 * _temperature) +
            (0.0000124 * _humidity);
        _logger.log("sfs", "Speed of sound = ", _speedOfSound, "mm/Ms");
    }

    void SensorHCSR04::_setEchoTimeout()
    {
        _echoTimeout = HCSR04_MAX_MM * 2 / _speedOfSound;
        _logger.log("sis", "Echo timeout = ", _echoTimeout, "Ms");
    }

    void SensorHCSR04::_takeSamples()
    {
        _successfulSamples = 0;
        for (int i = 0; i < HCSR04_SAMPLES_TAKE; i++)
        {
            // Because I notice some repeating patterns in timings when doing
            // a tight loop here, I add some random waits to get a better spread
            // of sample values.
            if (i > 0)
            {
                delay(HCSR04_SAMPLE_WAIT + random(HCSR04_SAMPLE_WAIT_SPREAD));
            }
            int distance = _takeSample();
            if (distance != -1)
            {
                _samples[i] = distance;
                _successfulSamples++;
            }
        }
    }

    bool SensorHCSR04::_haveEnoughSamples()
    {
        return _successfulSamples >= HCSR04_SAMPLES_USE;
    }

    int SensorHCSR04::_takeSample()
    {
        // Send 10μs trigger to ask sensor for a measurement.
        digitalWrite(HCSR04_TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(HCSR04_TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(HCSR04_TRIG_PIN, LOW);

        // Measure the length of echo signal.
        unsigned long durationMicroSec = pulseIn(HCSR04_ECHO_PIN, HIGH, _echoTimeout);

        // Compute the distance, based on the echo signal length.
        double distance = durationMicroSec / 2.0 * _speedOfSound;
        _logger.log("sfs", "Sample result = ", distance, "mm");
        if (distance < HCSR04_MIN_MM || distance >= HCSR04_MAX_MM)
        {
            return -1;
        }
        else
        {
            return distance;
        }
    }

    void SensorHCSR04::_sortSamples()
    {
        int holder, x, y;
        for (x = 0; x < _successfulSamples; x++)
        {
            for (y = 0; y < _successfulSamples - 1; y++)
            {
                if (_samples[y] > _samples[y + 1])
                {
                    holder = _samples[y + 1];
                    _samples[y + 1] = _samples[y];
                    _samples[y] = holder;
                }
            }
        }
    }

    // Compute the average of the samples. To get rid of measuring extremes,
    // only a subset of measurements from the middle are used.
    // When not enough samples were collected in the previous steps, then
    // NAN is returned.
    int SensorHCSR04::_computeAverage()
    {
        float sum = 0;
        int offset = (_successfulSamples - HCSR04_SAMPLES_USE) / 2;
        for (int i = 0; i < HCSR04_SAMPLES_USE; i++)
        {
            sum += _samples[i + offset];
        }

        return round(sum / HCSR04_SAMPLES_USE);
    }
}