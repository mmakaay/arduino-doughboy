#ifndef HCSR04_H
#define HCSR04_H

// The minimum and maximum distance that can be measured in mm.
// This is based on the specifications of the HCSR04 sensor.
#define HCSR04_MIN_MM 40
#define HCSR04_MAX_MM 4000

// Some parameters that are used to get more stable reading from the sensor.
// To get a better reading:
// - multiple samples are taken
// - between each sample, a random wait is added (because I saw repeating
//   patterns when reading from a tight loop)
// - only a subset from the samples is used to compute the average distance
//   (the high and low extremes are ignored)
#define HCSR04_SAMPLES_TAKE 20
#define HCSR04_SAMPLES_USE 8
#define HCSR04_SAMPLE_WAIT 30
#define HCSR04_SAMPLE_WAIT_SPREAD 12

// Default values for temperature and humidity, which have an effect
// on the speed of sound. At runtime, the temperature and humidity
// can be modified by using their respective setter functions
// setTemperature() and setHumidity().
#define HCSR04_INIT_TEMPERATURE 19.000
#define HCSR04_INIT_HUMIDITY 50.000

// Define this one to enable HCSR04 debug logging.
#undef HCSR04_DEBUG

#include <Arduino.h>
#include "UI/Logger.h"
#include "config.h"

namespace Dough
{
    // This class is used to get a distance reading from an HCSR04 sensor.
    class SensorHCSR04
    {
    public:
        SensorHCSR04(int triggerPin, int echoPin);
        void setup();
        void setTemperature(int temperature);
        void setHumidity(int humidity);
        int readDistance();
        int precision;

    private:
        Logger _logger;
        int _triggerPin;
        int _echoPin;
        int _humidity;
        int _temperature;
        void _setSpeedOfSound();
        float _speedOfSound;
        void _setEchoTimeout();
        int _echoTimeout;
        float _samples[HCSR04_SAMPLES_TAKE];
        void _takeSamples();
        bool _haveEnoughSamples();
        int _takeSample();
        int _successfulSamples;
        void _sortSamples();
        int _computeAverage();
    };
}

#endif