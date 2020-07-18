#include "HumiditySensor.h"

namespace Dough
{
    HumiditySensor::HumiditySensor() : _logger(getName()) {}

    void HumiditySensor::setup()
    {
        SensorDHT11::Instance()->begin();
    }

    const char* HumiditySensor::getName()
    {
        return "humidity";
    }

    Measurement HumiditySensor::read()
    {
        float t = SensorDHT11::Instance()->readHumidity();
        if (t == -1)
        {
            _logger.log("s", "ERROR - Humidity measurement failed");
            return Measurement::Failed();
        }
        else
        {
            _logger.log("sis", "Humidity = ", int(t), "%");
            return Measurement::Value(int(t));
        }
    }

    unsigned int HumiditySensor::getPrecision()
    {
        return 2; // prevent flapping when transitioning from value A to value B
    }
} // namespace Dough