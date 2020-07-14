#include "DistanceSensor.h"

namespace Dough
{
    DistanceSensor *DistanceSensor::Instance()
    {
        static DistanceSensor *_instance = new DistanceSensor();
        return _instance;
    }

    DistanceSensor::DistanceSensor() : _logger("DISTANCE")
    {
        _logger.log("s", "INIT distance sensor object");// TODO
        _hcsr04 = new SensorHCSR04(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN);
    }

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

    unsigned int DistanceSensor::getPrecision()
    {
        return 3; // according to the sensor specifications
    }    
}