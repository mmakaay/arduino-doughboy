#include "TemperatureSensor.h"

namespace Dough
{
    TemperatureSensor *TemperatureSensor::Instance()
    {
        static TemperatureSensor *_instance = new TemperatureSensor();
        return _instance;
    }

    TemperatureSensor::TemperatureSensor() : _logger("TEMPERATURE") {}

    void TemperatureSensor::setup()
    {
        SensorDHT11::Instance()->begin();
    }

    // ----------------------------------------------------------------------
    // loop
    // ----------------------------------------------------------------------

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
            DistanceSensor::Instance()->setTemperature(int(t));
            return Measurement::Value(int(t));
        }
    }

    unsigned int TemperatureSensor::getPrecision()
    {
        return 2; // prevent flapping when transitioning from value A to value B
    }
}