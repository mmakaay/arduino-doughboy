#include "Sensors/LowLevel/SensorDHT11.h"

namespace Dough
{
    // I am using a singleton here, to make it possible to use the physical
    // DHT11 sensor from the two logical sensors TemperatureSensor and
    // HumiditySensor.
    SensorDHT11 *SensorDHT11::Instance()
    {
        static SensorDHT11 *_instance = new SensorDHT11();
        return _instance;
    }

    SensorDHT11::SensorDHT11()
    {
        _dht = new DHT(DHT11_DATA_PIN, DHT11);
    }

    void SensorDHT11::begin()
    {
        static bool begun = false;
        if (!begun)
        {
            begun = true;
            _dht->begin();
        }
    }

    float SensorDHT11::readHumidity()
    {
        return _dht->readHumidity();
    }

    float SensorDHT11::readTemperature()
    {
        return _dht->readTemperature();
    }
} // namespace Dough