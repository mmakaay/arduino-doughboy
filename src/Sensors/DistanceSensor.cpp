#include "DistanceSensor.h"

namespace Dough
{
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    DistanceSensor *DistanceSensor::_instance = nullptr;

    DistanceSensor *DistanceSensor::Instance()
    {
        if (DistanceSensor::_instance == nullptr)
        {
            DistanceSensor::_instance = new DistanceSensor();
        }
        return DistanceSensor::_instance;
    }

    DistanceSensor::DistanceSensor() : _logger("DISTANCE")
    {
        _hcsr04 = new SensorHCSR04(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN);
    }

    // ----------------------------------------------------------------------
    // setup
    // ----------------------------------------------------------------------

    void DistanceSensor::setup()
    {
        _hcsr04->setup();
    }

    void DistanceSensor::setTemperature(int temperature)
    {
        _hcsr04->setTemperature(temperature);
    }

    void DistanceSensor::setHumidity(int humidity)
    {
        _hcsr04->setHumidity(humidity);
    }

    // ----------------------------------------------------------------------
    // loop
    // ----------------------------------------------------------------------

    Measurement DistanceSensor::read()
    {
        int d = _hcsr04->readDistance();
        if (d == -1)
        {
            _logger.log("s", "ERROR - Distance measurement failed");
            return Measurement::Failed();
        }
        else
        {
            _logger.log("sis", "Distance = ", d, "mm");
            return Measurement::Value(d);
        }
    }
} // namespace Dough