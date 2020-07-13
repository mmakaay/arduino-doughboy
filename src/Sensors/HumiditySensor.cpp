#include "HumiditySensor.h"

namespace Dough
{
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    HumiditySensor *HumiditySensor::Instance()
    {
        static HumiditySensor *_instance = new HumiditySensor();
        return _instance;
    }

    HumiditySensor::HumiditySensor() : _logger("HUMIDITY") {}

    // ----------------------------------------------------------------------
    // setup
    // ----------------------------------------------------------------------

    void HumiditySensor::setup()
    {
        SensorDHT11::Instance()->begin();
    }

    // ----------------------------------------------------------------------
    // loop
    // ----------------------------------------------------------------------

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
            DistanceSensor::Instance()->setHumidity(int(t));
            return Measurement::Value(int(t));
        }
    }

    unsigned int HumiditySensor::getPrecision()
    {
        return 2; // prevent flapping when transitioning from value A to value B
    }    
}