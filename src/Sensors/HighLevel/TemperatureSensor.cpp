#include "TemperatureSensor.h"

namespace Dough
{
    TemperatureSensor::TemperatureSensor() : _logger(getName()) {}

    void TemperatureSensor::setup()
    {
        SensorDHT11::Instance()->begin();
    }

    const char* TemperatureSensor::getName()
    {
        return "temperature";
    }

    Measurement TemperatureSensor::read()
    {
        float t = SensorDHT11::Instance()->readTemperature();
        if (isnan(t))
        {
            _logger.log("s", "ERROR - Temperature measurement failed");
            return Measurement::Failed();
        }
        else
        {
            _logger.log("sis", "Temperature = ", int(t), "°C");
            return Measurement::Value(int(t));
        }
    }

    unsigned int TemperatureSensor::getPrecision()
    {
        return 2; // prevent flapping when transitioning from value A to value B
    }
}