#include "Sensors/LowLevel/SensorDHT11.h"

namespace Dough
{
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    SensorDHT11 *SensorDHT11::_instance = nullptr;

    SensorDHT11 *SensorDHT11::Instance()
    {
        if (SensorDHT11::_instance == nullptr)
        {
            SensorDHT11::_instance = new SensorDHT11();
        }
        return SensorDHT11::_instance;
    }

    SensorDHT11::SensorDHT11()
    {
        _dht = new DHT(DHT11_DATA_PIN, DHT11);
    }

    // ----------------------------------------------------------------------
    // setup
    // ----------------------------------------------------------------------

    void SensorDHT11::begin()
    {
        _dht->begin();
    }

    // ----------------------------------------------------------------------
    // loop
    // ----------------------------------------------------------------------

    float SensorDHT11::readHumidity()
    {
        return _dht->readHumidity();
    }

    float SensorDHT11::readTemperature()
    {
        return _dht->readTemperature();
    }
}