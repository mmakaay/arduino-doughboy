#ifndef DOUGH_SENSOR_DHT11_H
#define DOUGH_SENSOR_DHT11_H

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
        DHT *_dht;
    };
}

#endif