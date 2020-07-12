#ifndef DOUGH_SENSORS_DHT11_H
#define DOUGH_SENSORS_DHT11_H

#include <DHT.h>
#include "config.h"

namespace Dough
{
    // This class provides access to the DHT11 sensor in the device.
    class SensorDHT11
    {
    public:
        static SensorDHT11 *Instance();
        void begin();
        float readTemperature();
        float readHumidity();

    private:
        SensorDHT11();
        static SensorDHT11 *_instance;
        DHT *_dht;
    };
}

#endif