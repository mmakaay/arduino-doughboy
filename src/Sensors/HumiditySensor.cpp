#include "HumiditySensor.h"

namespace Dough
{
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    HumiditySensor *HumiditySensor::_instance = nullptr;

    HumiditySensor *HumiditySensor::Instance()
    {
        if (HumiditySensor::_instance == nullptr)
        {
            HumiditySensor::_instance = new HumiditySensor();
        }
        return HumiditySensor::_instance;
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
} // namespace Dough